#ifndef __MATCH_CONDITION__
#define __MATCH_CONDITION__

struct Match_Condition
{
    float MAX_AREA_RATIO_DIFF = 0.5;
    float MAX_PERIMRTER_RATIO_DIFF = 0.6; 
    float MAX_WH_RATIO = 1;
    float MAX_ANGLE_DIFF = 10;
    float MIN_AREA_FULL = 0.5;
    float MAX_WIDTH_RATIO_DIFF = 0.3;
    float MAX_HEIGHT_RATIO_DIFF = 0.3;
    float MAX_SLOPE_DIFF = 0.3;
};

#endif // !__MATCH_CONDITION__