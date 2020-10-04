#ifndef __MATCH_CONDITION__
#define __MATCH_CONDITION__

struct Match_Condition
{
    float MAX_AREA_RATIO_DIFF = 0.55;
    float MAX_WH_RATIO = 1.2;
    float MAX_ANGLE_DIFF = 30;
    float MIN_AREA_FULL = 0.5;
    float MAX_WIDTH_RATIO_DIFF = 0.4;
    float MAX_HEIGHT_RATIO_DIFF = 0.4;
};

#endif // !__MATCH_CONDITION__