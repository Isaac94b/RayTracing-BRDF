#ifndef _SOURCE_H
#define _SOURCE_H

class Source {
	public:
	
	Source();
	
	virtual Vect getLightPosition() {return Vect(0, 0, 0);}
	virtual Color getLightColor() {return Color(1, 1, 1, 0);}

	virtual Color getLight_Ambient_Intensity() { return Color(1,1,1,0);}
     virtual Color getLight_Diffuse_Intensity() { return Color(1,1,1,0);}
     virtual Color getLight_Specular_Intensity() { return Color(1,1,1,0);}
	
};

Source::Source() {}

#endif
