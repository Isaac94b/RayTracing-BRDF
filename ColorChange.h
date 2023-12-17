Color HSV_Type(Color Final_Color){

	 Color tmpr_final;

    double H  = pow((1/2*((Final_Color.getColorRed()-Final_Color.getColorGreen())+(Final_Color.getColorRed()-Final_Color.getColorBlue())))/
                    (sqrtf(pow(Final_Color.getColorRed()-Final_Color.getColorGreen(),2)
                           + ((Final_Color.getColorRed()-Final_Color.getColorBlue())*(Final_Color.getColorGreen()-Final_Color.getColorBlue())))),-1);


    double min= 0;

    if (Final_Color.getColorRed() < Final_Color.getColorGreen())
    {

        min = Final_Color.getColorRed();
    }else {

        min = Final_Color.getColorGreen();
    }

    if (Final_Color.getColorBlue() < min )
    {
        min = Final_Color.getColorBlue();
    }

    double S = 1 - ((3/(Final_Color.getColorRed()+Final_Color.getColorGreen()+Final_Color.getColorBlue())) * min);


    double V = 1 / 3 * (Final_Color.getColorRed() + Final_Color.getColorGreen() + Final_Color.getColorBlue());


    tmpr_final.setColorRed(H);
    tmpr_final.setColorGreen(S);
    tmpr_final.setColorBlue(V);

    return tmpr_final;
}

Color YIQ_Type(Color Final_Color){


	Color tmpr_final ;

    tmpr_final.setColorRed(0.299 * Final_Color.getColorRed() + 0.587 * Final_Color.getColorBlue() + 0.114 * Final_Color.getColorGreen());
    tmpr_final.setColorGreen(0.596 * Final_Color.getColorRed() + -0.274 * Final_Color.getColorBlue() + 0.322 * Final_Color.getColorGreen());
    tmpr_final.setColorBlue(0.211 * Final_Color.getColorRed() + -0.523 * Final_Color.getColorBlue() +  0.312 * Final_Color.getColorGreen());


    return tmpr_final;


}

Color YUV_Type(Color Final_Color)
{

    Color tmpr_final ;

    tmpr_final.setColorRed(0.299 * Final_Color.getColorRed() + 0.587 * Final_Color.getColorBlue() + 0.114 * Final_Color.getColorGreen());
    tmpr_final.setColorGreen(-0.14713 * Final_Color.getColorRed() + -0.28886 * Final_Color.getColorBlue() + 0.436 * Final_Color.getColorGreen());
    tmpr_final.setColorBlue(0.615 * Final_Color.getColorRed() + -0.51499 * Final_Color.getColorBlue() + -0.10001 * Final_Color.getColorGreen());


    return tmpr_final;

}

Color XYZ_Type(Color Final_Color)
{
    Color tmpr_final ;


    tmpr_final.setColorRed(0.57667 * Final_Color.getColorRed() + 0.18556 * Final_Color.getColorBlue() + 0.18823 * Final_Color.getColorGreen());
    tmpr_final.setColorGreen(0.29734 * Final_Color.getColorRed() + 0.62736 * Final_Color.getColorBlue() + 0.07529 * Final_Color.getColorGreen());
    tmpr_final.setColorBlue(0.02703 * Final_Color.getColorRed() + 0.07069 * Final_Color.getColorBlue() + 0.99134 * Final_Color.getColorGreen());

    return tmpr_final;


}


Color YCbCr_Type(Color Final_Color)
{

    Color tmpr_final ;

    tmpr_final.setColorRed(0.299 * Final_Color.getColorRed() + 0.587 * Final_Color.getColorBlue() + 0.114 * Final_Color.getColorGreen());
    tmpr_final.setColorGreen(-0.1687 * Final_Color.getColorRed() + -0.3313 * Final_Color.getColorBlue() + 0.5000 * Final_Color.getColorGreen());
    tmpr_final.setColorBlue(0.5000 * Final_Color.getColorRed() + -0.4187 * Final_Color.getColorBlue() +  -0.0813 * Final_Color.getColorGreen());


    return tmpr_final;

}