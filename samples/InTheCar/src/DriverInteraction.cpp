
///
/// \file DriverInteraction.cpp
///
///      Created on: May 27, 2015
///     Author: alopez
///

#include "DriverInteraction.hpp"
#include <exception>

gestoos::nui::DriverInteraction::DriverInteraction()
{
	_frame=0;
	_draw_window=false;
	_use_hand_gestures=false;
	_use_tracker=false;
	_hand_id=-1;
}

gestoos::nui::DriverInteraction::~DriverInteraction()
{
    
}

void gestoos::nui::DriverInteraction::init(const std::string& ini_file)
{
	//Configure camera
	_capture.init("",  0,  gestoos::CaptureRGBD::QVGA_30FPS);
    //_capture.init("/Users/alopez/workspace/fezoolib/CarScenario4.oni", 0, gestoos::CaptureRGBD::QVGA_30FPS);

	//Load gestures and trackers
	_config.load(ini_file);

	//Get depth range
	std::pair<int, int> depth_range;
	depth_range.first=200;
	depth_range.second=1000;

	if (_config.get_range().min > 0 && _config.get_range().min <  _config.get_range().max  )
	{
		depth_range.first=_config.get_range().min;
	}
	if (_config.get_range().max < 5000 && _config.get_range().min <  _config.get_range().max  )
	{
		depth_range.second=_config.get_range().max;
	}

	//Tracker
	if ( _config.get_tracker().model!="" )
	{
		if (_config.get_tracker().tracking_threshold > 0.)
			_whai.init(_config.get_tracker().model, _config.get_tracker().threshold, depth_range, _config.get_tracker().tracking_threshold);
		else
			_whai.init(_config.get_tracker().model, _config.get_tracker().threshold, depth_range);

		_use_tracker=true;
	}
	//Hand gestures

	std::size_t num_hand_gestures = _config.get_hand_gestures().size();
	std::vector<int> transition_labels;
	std::vector<int> transition_from;
	std::vector<int> transition_to;
	if (num_hand_gestures)
	{
		_use_hand_gestures=true;
		std::vector<std::string> models;
		std::vector<double> thresholds;
		std::vector<int> labels;
		std::vector<std::string> usages;
		std::vector<int> complexities;

		for (std::size_t i=0; i < num_hand_gestures; ++i)
		{
			std::vector< gestoos::nui::Config::HandGesture>::const_iterator iter;
			const std::vector< gestoos::nui::Config::HandGesture> &  h = _config.get_hand_gestures();

			if (_config.get_hand_gestures()[i].model=="" && _config.get_hand_gestures()[i].from!="")
			{

				int from, to;
				if (_config.get_hand_gestures()[i].from=="ANY")
				{
					from = gestoos::nui::ANY;
				}
				else
				{
					//Find the gesture with the given label
					for (iter=h.begin(); iter!=h.end(); ++iter)
					{
						if (iter->name==h[i].from)
						{
							from=iter->label;
							break;
						}
					}
				}
				if (_config.get_hand_gestures()[i].to=="ANY")
				{
					to = gestoos::nui::ANY;
				}
				else
				{
					//Find the gesture with the given label
					for (iter=h.begin(); iter!=h.end(); ++iter)
					{
						if (iter->name==h[i].to)
						{
							to=iter->label;
							break;
						}
					}
				}

				transition_labels.push_back(h[i].label);
				transition_from.push_back(from);
				transition_to.push_back(to);
			}
			else
			{
				models.push_back(h[i].model);
				labels.push_back(h[i].label);
				thresholds.push_back(h[i].threshold);
				usages.push_back(h[i].usage);
				complexities.push_back(h[i].complexity);
			}
		}

		_hand_detector.set_video_mode(gestoos::CaptureRGBD::QVGA_30FPS);
		_hand_detector.init_detector(models, labels, thresholds, usages, complexities, depth_range); //applications/commands/valcriDemo");
		_hand_detector.set_sampling_downscale(5);
		_hand_detector.use_motion_detection(false);
		_hand_detector.activate_multithreading(true);
		_mask.create(cv::Size(320, 240), CV_8UC1);

		_hand.init(_hand_detector.get_labels(), _hand_detector.get_thresholds());


		for (std::size_t j=0; j < transition_labels.size(); ++j)
		{
			std::cout << "Calling add_hand_gesture_on_transition " << transition_labels[j] << " " <<  transition_from[j] << " " <<  transition_to[j]<< std::endl;
			this->_add_hand_gesture_on_transition(transition_labels[j], transition_from[j], transition_to[j]);
		}
	}

	if (!_use_hand_gestures)
	{
		throw std::runtime_error("Driver Interaction Exception : You should use at least hand gestures to interact");
	}
}

void gestoos::nui::DriverInteraction::process()
{
	/*
	 * Capture
	 */
	_capture.get_depth_frame();
	_depth_map = _capture.depth_frame();

	/*
	 * Filter data, mainly filling depth holes
	 */
	_depth_map=gestoos::depth_error_filter(_depth_map);

	//TODO: For the moment we do not limit the tracker to the interaction space
	if (_use_tracker)
	{
		_whai.update(_depth_map, _frame);

		//Associate hand trajectories
		std::vector<gestoos::tracking::ObjectTrack*> objects= _whai.active_tracks();

		for (unsigned int i =0; i < objects.size(); ++i)
		{
			int idx = objects[i]->get_id();
			if (idx==_hand_id) //Hand update
			{
				_hand.set_pos(objects[i]->get_position());
			}
			else //Hand appearing
			{
                std::cout << "Hand Appearing ... " << std::endl;
				_hand.clear();
				//Note: interaction space is automatically set with set_pos
                _hand_id = idx;
				_hand.set_pos(objects[i]->get_position());
                
			}
		}

//		//From tracker outputs, process gestures
//        _mask=cv::Scalar(0);
//        if (_hand.is_present())
//        {
//
//            cv::Point p(_hand.get_pos().x,_hand.get_pos().y);
//            double depth = gestoos::get_reference_depth(_depth_map, p, 5);
//            cv::Rect roi = gestoos::generate_bounding_box_depth_scaled(p,  depth, cv::Size(61, 61));
//            _mask(gestoos::crop_to_image(roi, _mask))=cv::Scalar(255);
//
//            //Bitwise and with the scene mask, if any
//            if (!_scene_mask.empty())
//            {
//                if (_scene_mask.size()==_mask.size())
//                {
//                    cv::bitwise_and(_scene_mask, _mask, _mask);
//                                    }
//                else
//                {
//                    throw std::runtime_error("[Driver Interaction] Scene mask and inner mask are not of the same size");
//                }
//            }
//
//
//        }
        _mask=_scene_mask.clone();

        /*
         * Gesture detection after tracking
         */
        
        //Filter the scene mask
        _depth_map=_capture.depth_frame(); //Re-copy the depth frame
        _fmm.set_alpha(0.1);
        _fmm.filter(_depth_map, _scene_mask);
        _hand_detector.process(_depth_map, _mask);
        
        //This transfers the gesture to the hand
        _hand.process(_depth_map, _hand_detector);

#ifndef AVOID_QT
			if( _draw_window )
			{

                //Visualize tracking detection
                gestoos::scoreHeatMap(_whai.get_detector_map(), _color_img, 0, 10);
                cv::imshow("HandScore", _color_img);
                //Visualize tracking detection
                gestoos::scoreHeatMap(_hand_detector.get_probability_map(2), _color_img, 0, 10);
                cv::imshow("HandGestureScore", _color_img);
				//DBG: generating visualization of tracking
				gestoos::falseColorMap(_depth_map, _color_img, cv::COLORMAP_HOT);

                cv::Mat aux;
                gestoos::falseColorMap(_depth_map, aux, cv::COLORMAP_BONE);
                if (!_scene_mask.empty())
                    aux.copyTo(_color_img, _scene_mask);
                else{
                    _color_img=aux.clone();
                }
                
                
				if (_hand.get_interaction_space().area() > 0)
				{
					cv::rectangle(_color_img, _hand.get_interaction_space(), cv::Scalar(255, 255, 255), 3);
					cv::rectangle(_color_img, _hand.get_interaction_space(), cv::Scalar(0, 255, 0), 2);
				}
				_whai.visualize(this->_color_img, 1, false);
				cv::imshow("Tracker", _color_img);
				cv::waitKey(10);
			}
#endif

	}
	else
	{
		//Purely gesture based?
		std::cout << "No tracker " << std::endl;
	}

}

int gestoos::nui::DriverInteraction::get_hand_gesture() const
{
    double maxVal;
    cv::Point maxLoc;
    double best_meta=1.;
    int best_gesture=-1;
    for (int i =0; i < _hand_detector.get_labels().size(); ++i)
    {
        const cv::Mat & scores = _hand_detector.get_probability_map(i+1);
        if (!scores.empty())
        {
            cv::minMaxLoc(scores , NULL, &maxVal, NULL, &maxLoc);
            double threshold = _hand_detector.get_thresholds()[i];
            
            //if (maxVal > threshold)
            //    return _hand_detector.get_labels()[0];
            
            if (maxVal/threshold > best_meta )
            {
                best_meta=maxVal/threshold;
                best_gesture=_hand_detector.get_labels()[i];
            }
        }
    }
    
    return best_gesture;
}

void gestoos::nui::DriverInteraction::stop()
{
	_capture.stop();
	_hand_detector.stop();
}

void gestoos::nui::DriverInteraction::set_resources_path(const std::string& resources)
{
	_config.set_resources_path(resources);
}


const  gestoos::nui::Config& gestoos::nui::DriverInteraction::get_config() const {
	return _config;
}

void gestoos::nui::DriverInteraction::set_config(const Config& config) {
	_config = config;
}

bool gestoos::nui::DriverInteraction::is_draw_window() const {
	return _draw_window;
}

void gestoos::nui::DriverInteraction::set_draw_window(bool drawWindow) {
	_draw_window = drawWindow;
}

const  gestoos::nui::Hand& gestoos::nui::DriverInteraction::get_hand() const {
	return _hand;
}

const cv::Mat& gestoos::nui::DriverInteraction::get_scene_mask() const {
	return _scene_mask;
}

void gestoos::nui::DriverInteraction::set_scene_mask(const cv::Mat& sceneMask) {
	_scene_mask = sceneMask;
}

void gestoos::nui::DriverInteraction::set_scene_mask(const std::string& frame)
{
	_scene_mask=cv::imread(frame, CV_LOAD_IMAGE_GRAYSCALE);
	//TODO: Assert 8 bit and 320x240
}

void gestoos::nui::DriverInteraction::_add_hand_gesture_on_transition(int label,
                                                                      int from, int to) {
    _hand.add_gesture_on_transition(label, from, to);
}
