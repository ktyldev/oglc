float getLinearDepth(float distance)
{
    return distance/INF;
}

float getLogarithmicDepth(float distance)
{
    // n roughly correlates to steepness of log curve
    // TODO: what does this mean in mathematical terms??
    float n = 4;
    float f = INF;
    float z = distance;
    
    // logarithmic depth
    return max(0,log(z*pow(E,n)/f)/n);
}
