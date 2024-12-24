#ifndef __KEPRI_SHADER__
#define __KEPRI_SHADER__

class Shader
{
public:
	Shader() : programId(0) {}

	void load(const char* vertexShaderSrc, const char* fragmentShaderSrc);

	inline unsigned int getId() const { return programId; }
private:
	unsigned int programId;
};

#endif