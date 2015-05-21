//
//  Cinderactor.h
//  InTheCar
//
//  Created by Mac on 21/05/15.
//
//

#ifndef InTheCar_Cinderactor_h
#define InTheCar_Cinderactor_h

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"

#include "fezoolib/NUI/Interactor.hpp"

using namespace ci;

static int     GEST_VICTORY    = 14;
static float   BLOCK_TIMEOUT   = 1.0;

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
    
    /// Detect hand strokes restricted to a given hand gest.
    /// If gest == -1, strokes are detected for any gesture.
    StrokeType detect_hand_stroke( int gest = -1 );
    
    /// Draw a cool representation of the interaction data.
    void draw() const;
    
private:
    cinder::Timer block_timer;

    
};

#endif
