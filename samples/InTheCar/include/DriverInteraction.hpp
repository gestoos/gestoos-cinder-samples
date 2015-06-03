// --------------------------------------------------------------
// Copyright (C)
// FEZOO LABS SL
// --------------------------------------------------------------

/*

 <LicenseNote>

 */

///
/// \file DriverInteraction.hpp
///
///      Created on: May 27, 2015
///     Author: alopez
///

#ifndef GESTOOS_NUI_DRIVERINTERACTION_HPP_
#define GESTOOS_NUI_DRIVERINTERACTION_HPP_


#include <fezoolib/NUI/Config.hpp>
#include <fezoolib/NUI/Hand.hpp>

namespace gestoos{
	namespace nui{

		/// \brief Class for gesture interaction for car drivers
		///
		/// This class assumes a zenital depth sensor in the ceiling of the car
		class DriverInteraction
		{
		public:

			DriverInteraction();

			~DriverInteraction();

			/// \brief Init from a unified file
			void init(const std::string  & ini_file);

			/// \brief Main processing method. To called continuosly after init;
			void process();

			/// \brief Stop method, to be called before exiting an application, making sure the sensors are properly shut down
            void stop();
            
            /// \brief Set the resources path
            void set_resources_path(const std::string& resources);

            /// Accessors
			const Config& get_config() const;
			void set_config(const Config& config);

			bool is_draw_window() const;
			void set_draw_window(bool drawWindow);

			const Hand& get_hand() const;

			const cv::Mat& get_scene_mask() const;
			/// Set a 320x240, 8 bit, single channel binary mask delimiting the 2D scenario (i.e., the ROI where gestures can be performed)
			void set_scene_mask(const cv::Mat& sceneMask);

			/// Set scene mask from file
			///
			/// PNG or uncompressed formats are suggested
			void set_scene_mask(const std::string & frame);

		private:

			//Test on a live device
			gestoos::CaptureRGBD _capture;

			//Declare WHAITracker
			gestoos::tracking::WHAITracker _whai;

			//Hand Gesture Detector
			gestoos::detection::GestureDetector _hand_detector;

			//To store the current frame num
			gestoos::tracking::ObjectTrack::ts_type _frame;

			//Scene mask, limiting the interaction area
			cv::Mat _scene_mask;

			/*
			 * Tracking inputs
			 */
			cv::Mat _depth_map;
			cv::Mat _color_img;
			cv::Mat _mask;

			/*
			 * Hand identifier
			 */

			Hand _hand;
			int _hand_id;

			//Configuration
			Config _config;
			bool _draw_window;
			bool _use_tracker;
			bool _use_hand_gestures;

			void _add_hand_gesture_on_transition(int label, int from, int to);

		};

	}
}

#endif /* DRIVERINTERACTION_HPP_ */
