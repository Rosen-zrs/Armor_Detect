#ifndef __MATCH_CONDITION__
#define __MATCH_CONDITION__

struct Match_Condition
{
    float MAX_AREA_RATIO_DIFF = 1;
    float MAX_PERIMRTER_RATIO_DIFF = 1; 
    float MAX_ANGLE_DIFF = 20;
    float MAX_WIDTH_RATIO_DIFF = 0.5;
    float MAX_HEIGHT_RATIO_DIFF = 0.5;
};

#endif // !__MATCH_CONDITION__