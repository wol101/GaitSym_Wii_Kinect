#include "colors.inc"
#include "textures.inc"

global_settings { max_trace_level 16 }

// light source and background definitions

light_source {<0, 0, 100> color Gray80 shadowless parallel point_at <0, 0, 0> }
light_source {<100, -100, 100> color White parallel point_at <0, 0, 0> }

// Right-handed coordinates with z up
camera {
    perspective
    location   < gCameraX + 3, gCameraY, gCameraZ + 1.0 >
    //direction  < 0, 1, 0 >
    up         < 0.0, 0.0, 1.0 >
    sky        < 0.0, 0.0, 1.0 >
    right      < image_width / image_height, 0.0, 0.0 >
    angle      5.0
    look_at    < gCOIx, gCOIy, gCOIz >
}

// sphere {
//    <0, 0, 0>, 500
//    texture { Bright_Blue_Sky }
//    hollow
// }

// Put down a  floor
// plane {
//     <0, 0, 1>, 0
//     
//     pigment { checker pigment{ White }, pigment{ Gray90 } }
//     hollow
// }

box
{
    <-200, -200, -1>, <200, 200, 0>
    pigment { checker pigment{ White }, pigment{ Gray90 } }
}

box
{
    <-200, 10, -1>, <200, 11, 200>
    pigment { checker pigment{ White }, pigment{ Gray90 } }
}
