#ifndef _Light_H
#define _Light_H

#include "Source.h"
#include "Vect.h"
#include "Color.h"

class Light : public Source {
	Vect position;
	Color color;
	Color Light_Ambient_Intensity;
    Color Light_Diffuse_Intensity;
    Color Light_Specular_Intensity;

	
	public:
	
	Light ();
	
	Light (Vect, Color);

	Light(Vect, Color, Color, Color);
	
	// method functions
	virtual Vect getLightPosition () { return position; }
	virtual Color getLightColor () { return color; }
	virtual Color getLight_Ambient_Intensity() { return Light_Ambient_Intensity;}
    virtual Color getLight_Diffuse_Intensity() { return Light_Diffuse_Intensity;}
    virtual Color getLight_Specular_Intensity() { return Light_Specular_Intensity;}
	
};

Light::Light () {
	position = Vect(0,0,0);
	color = Color(1,1,1, 0);
	Light_Ambient_Intensity = Color(0,0,0, 0);
    Light_Diffuse_Intensity = Color(0,0,0, 0);
    Light_Specular_Intensity = Color(0,0,0,0);
}

Light::Light (Vect p, Color c) {
	position = p;
	color = c;
}

Light::Light (Vect p, Color Light_A_I_Value, Color Light_D_I_Value, Color Light_S_I_Value){
    
    position = p;
    color = Color(0,0,0, 0);
    Light_Ambient_Intensity = Light_A_I_Value;
    Light_Diffuse_Intensity = Light_D_I_Value;
    Light_Specular_Intensity = Light_S_I_Value;

    

}

#endif
