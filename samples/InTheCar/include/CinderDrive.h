//
//  CinderDrive.h
//  InTheCar
//
//  Created by Adolfo López Méndez on 28/05/15.
//
//

#ifndef InTheCar_CinderDrive_h
#define InTheCar_CinderDrive_h


#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"

//Make sure it is picked from the local project drive
#include "DriverInteraction.hpp"
#include "Cinderactor.h"

using namespace ci;

/// \brief This class extends gestoos::nui::Interactor, adding new cool features for Cinder apps.
class CinderDrive : public gestoos::nui::DriverInteraction
{
    
public:
    
    
    /// The hand stroke direction
    enum StrokeType {UP,DOWN,LEFT,RIGHT,NONE} ;
    
    /// Empty constructor
    CinderDrive() ;
    
    /// Base destructor
    ~CinderDrive();
    
    void init(const std::string & ini_file );
    
    /// Detects any positive hand gesture and throws a timeout afterwards
    int detect_hand_gesture(float timeout = 1.0);
    
    /// Detect hand strokes restricted to a given hand gest.
    /// If gest == -1, strokes are detected for any gesture.
    StrokeType detect_hand_stroke( int gest = -1, float timeout = 0.0 );
    
    /// Draw a cool representation of the interaction data.
    void draw() const;
    
    ///
    void draw_hand_circle( const gestoos::nui::Hand & hand ) const;
    
    ///
    static void draw_hand_representation( const gestoos::nui::Hand & hand, const Vec2f & where = Vec2f(70,70)  ) ;
    
private:
    cinder::Timer block_timer;
    bool init_ok;
    
};

#endif
