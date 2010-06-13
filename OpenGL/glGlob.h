
#include "../MyLib/Vec.h" 

#include <gl/gl.h>
#include <QString>

extern void mglCheckErrors(const char* place = NULL);
extern void mglCheckErrorsC(const char* place = NULL);
extern void mglCheckErrors(const QString& s);
extern void mglCheckErrorsC(const QString& s);

class SwitchColor
{
public:
	SwitchColor(const Vec3& col, const Vec3& oldcol) :m_oldc(oldcol)
	{
		glColor3fv(col.v);
	}
	~SwitchColor()
	{
		glColor3fv(m_oldc.v);
	}

private:
	Vec3 m_oldc;
};