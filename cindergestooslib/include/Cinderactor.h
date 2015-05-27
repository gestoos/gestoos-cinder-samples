//
//  Cinderactor.h
//  ActiveEdge
//
//  Created by Mac on 21/05/15.
//
//

#ifndef ActiveEdge_Cinderactor_h
#define ActiveEdge_Cinderactor_h

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"

#include "fezoolib/NUI/Interactor.hpp"

using namespace ci;

static int     GEST_AUDIO       = 4;

static int     GEST_EL          = 13;
static int     GEST_VICTORY     = 14;
static int     GEST_OPEN        = 15;
static int     GEST_CLOSE       = 16;

static int     GEST_GRAB        = 206;
static int     GEST_RELEASE     = 207;

/// \brief This class extends gestoos::nui::Interactor, adding new cool features for Cinder apps.
class Cinderactor : public gestoos::nui::Interactor
{

public:
    
   
    /// The hand stroke direction
    enum StrokeType {UP,DOWN,LEFT,RIGHT,NONE} ;
    
    /// Empty constructor
    Cinderactor() ;
    
    /// Base destructor
    ~Cinderactor();
    
    void init(const std::string & ini_file );
    
    /// Detect hand strokes restricted to a given hand gest.
    /// If gest == -1, strokes are detected for any gesture.
    StrokeType detect_hand_stroke( int gest = -1, float timeout = 0.0 );
    
    /// Draw a cool representation of the interaction data.
    void draw() const;
    
    ///
    void draw_hand_circle( const gestoos::nui::Hand & hand ) const;

    ///
    static void draw_hand_representation( const gestoos::nui::Hand & hand, const Vec2f & where = Vec2f(70,70)  ) ;

	///
	bool get_init_ok();
    
private:
    cinder::Timer block_timer;
    bool init_ok;
    
};

#endif
