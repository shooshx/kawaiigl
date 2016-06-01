#include "NoiseGenerator.h"
#include "OpenGL/Texture.h"

#include <cmath>



#define MAXB 0x100
#define N 0x1000
#define NP 12   // 2^N
#define NM 0xfff

#define s_curve(t) ( t * t * (3. - 2. * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup(i, b0, b1, r0, r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;
#define at2(rx, ry) ( rx * q[0] + ry * q[1] )
#define at3(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

static void initNoise();

static int p[MAXB + MAXB + 2];
static double g3[MAXB + MAXB + 2][3];
static double g2[MAXB + MAXB + 2][2];
static double g1[MAXB + MAXB + 2];

int start;
int B;
int BM;



void setNoiseFrequency(int frequency)
{
	start = 1;
	B = frequency;
	BM = B-1;
}

double noise1(double arg)
{
	int bx0, bx1;
	double rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);

	sx = s_curve(rx0);
	u = rx0 * g1[p[bx0]];
	v = rx1 * g1[p[bx1]];

	return(lerp(sx, u, v));
}

double noise2(double vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

	q = g2[b00]; u = at2(rx0, ry0);
	q = g2[b10]; v = at2(rx1, ry0);
	a = lerp(sx, u, v);

	q = g2[b01]; u = at2(rx0, ry1);
	q = g2[b11]; v = at2(rx1, ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

double noise3(double vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);
	setup(2, bz0, bz1, rz0, rz1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	q = g3[b00 + bz0]; u = at3(rx0, ry0, rz0);
	q = g3[b10 + bz0]; v = at3(rx1, ry0, rz0);
	a = lerp(t, u, v);

	q = g3[b01 + bz0]; u = at3(rx0, ry1, rz0);
	q = g3[b11 + bz0]; v = at3(rx1, ry1, rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[b00 + bz1]; u = at3(rx0, ry0, rz1);
	q = g3[b10 + bz1]; v = at3(rx1, ry0, rz1);
	a = lerp(t, u, v);

	q = g3[b01 + bz1]; u = at3(rx0, ry1, rz1);
	q = g3[b11 + bz1]; v = at3(rx1, ry1, rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

void normalize2(double v[2])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void normalize3(double v[3])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

void initNoise()
{
	int i, j, k;

	srand(30757);
	for (i = 0; i < B; i++)
	{
		p[i] = i;
		g1[i] = (double)((rand() % (B + B)) - B) / B;

		for (j = 0; j < 2; j++)
			g2[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0; j < 3; j++)
			g3[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0; i < B + 2; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0; j < 2; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0; j < 3; j++)
			g3[B + i][j] = g3[i][j];
	}
}

// My harmonic summing functions - PDB

//
// In what follows "alpha" is the weight when the sum is formed.
// Typically it is 2, As this approaches 1 the function is noisier.
// "beta" is the harmonic scaling/spacing, typically 2.
//

double PerlinNoise1D(double x,double alpha,double beta,int n)
{
	int i;
	double val,sum = 0;
	double p,scale = 1;

	p = x;
	for (i = 0; i < n; i++)
	{
		val = noise1(p);
		sum += val / scale;
		scale *= alpha;
		p *= beta;
	}
	return(sum);
}

double PerlinNoise2D(double x, double y, double alpha, double beta, int n)
{
	int i;
	double val, sum = 0;
	double p[2], scale = 1;

	p[0] = x;
	p[1] = y;
	for (i = 0; i < n; i++)
	{
		val = noise2(p);
		sum += val / scale;
		scale *= alpha;
		p[0] *= beta;
		p[1] *= beta;
	}
	return(sum);
}

double PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n)
{
	int i;
	double val,sum = 0;
	double p[3],scale = 1;

	p[0] = x;
	p[1] = y;
	p[2] = z;
	for (i = 0; i < n; i++)
	{
		val = noise3(p);
		sum += val / scale;
		scale *= alpha;
		p[0] *= beta;
		p[1] *= beta;
		p[2] *= beta;
	}
	return(sum);
}

#include <QProgressDialog>
#include <QApplication>

int ceilDiv(int x, int y) {
	return (x + y - 1)/y;
}

void saveto2D(uchar *data, int n, int depth, int inArow)
{
	int dmin = 260, dmax = -1;

	int iwidth = n * inArow;
	int iheight = (ceilDiv(depth, inArow)) * n; // should be up-div
	uchar *ni = new uchar[iwidth * iheight*4];
	int rowX = 0, rowY = 0, rowCount = 0;
	int nn = n*n;
	for(int z = 0; z < depth; ++z) {
		for(int y = 0; y < n; ++y) {
			for(int x = 0; x < n; ++x) {
				uchar *d = &data[(x + y*n + z*nn) * 4]; // take just the first
				int di = (x + rowX + (y + rowY)*iwidth)*4;
				ni[di] = *d;
				ni[di+1] = *(d+0);
				ni[di+2] = *(d+0);
				ni[di+3] = *(d+0);

				if (*d < dmin)
					dmin = *d;
				if (*d > dmax)
					dmax = *d;
			}
		}
		rowX += n;
		++rowCount;
		if (rowCount >= inArow) {
			rowX = 0;
			rowY += n;
			rowCount = 0;
		}
	}

	QImage img(ni, iwidth, iheight, QImage::Format_ARGB32);

	img.save("c:/temp/tex2d.jpg");

    img = img.mirrored(false, true);
    img.save("c:/temp/tex2d_flipped.jpg");

    // measure the difference for changing N_FACT
	printf("IntDelta= %d - %d = %d\n", dmax, dmin, dmax - dmin);
}

#define NUM_OCT 1
#define N_FACT 2.3
//#define N_FACT 1.0

// copied from the orange book
bool doNormalize = true;
GlTexture* NoiseGenerator::make3Dnoise(int size, float ampStart, float ampDiv, int startFrequency)
{
	int noise3DTexSize = size;
	GLubyte *noise3DTexPtr;

	int f, i, j, k, inc;
	//int startFrequency = 4;
	int numOctaves = NUM_OCT; //4;
	double ni[3];
	double inci, incj, inck;
	int frequency = startFrequency;
	GLubyte *ptr;
	double amp = ampStart;


	uint bufsize = noise3DTexSize * noise3DTexSize * noise3DTexSize * 4;
	if ((noise3DTexPtr = (GLubyte *) malloc(bufsize)) == NULL)
	{
		printf("ERROR: Could not allocate 3D noise texture - %d bytes\n", bufsize);
		return NULL;
	}

	QProgressDialog pdlg(QString("Generating noise size=%1").arg(size), "Stop", 0, numOctaves * noise3DTexSize, m_inWidget);
	pdlg.setMinimumDuration(500);

	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= ampDiv)
	{
		setNoiseFrequency(frequency);

        double normOffset = 0.0, normMult = 1.0;

        for(int iter = 0; iter < 2; ++iter) // once for finding min-max, second time to save normalized values
        {
            ptr = noise3DTexPtr;
            ni[0] = ni[1] = ni[2] = 0;
            double minn = 100.0, maxn = -100.0;

		    inci = 1.0 / (noise3DTexSize / frequency);
		    for (i = 0; i < noise3DTexSize; ++i, ni[0] += inci)
		    {

			    pdlg.setValue(noise3DTexSize * f + i);
			    QApplication::processEvents();
			    if (pdlg.wasCanceled())
				    return NULL;


			    incj = 1.0 / (noise3DTexSize / frequency);
			    for (j = 0; j < noise3DTexSize; ++j, ni[1] += incj)
			    {
				    inck = 1.0 / (noise3DTexSize / frequency);
				    for (k = 0; k < noise3DTexSize; ++k, ni[2] += inck, ptr+= 4)
				    {
					    double ns = noise3(ni) + 1.0;// * N_FACT;
                        ns = (ns + normOffset) * normMult;
					    *(ptr+inc) = (GLubyte)(((ns) * amp)*128.0); // 128.0
					    if (ns > maxn)
						    maxn = ns;
					    if (ns < minn)
						    minn = ns;
				    }
			    }
		    }

            printf("DblDelta= %lf - %lf = %lf\n", maxn, minn, maxn - minn);
            if (!doNormalize)
                break;

            normOffset = -minn;
            normMult = 2.0 / (maxn - minn); // 2 since we multiply by 128
        }
	}

	GlTexture *tex = new GlTexture();
	tex->init(GL_TEXTURE_3D, QSize(size, size), size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 
			 noise3DTexPtr, GL_LINEAR, GL_LINEAR, GL_REPEAT); //GL_NEAREST

	saveto2D(noise3DTexPtr, size, size, 8); 

	free(noise3DTexPtr);

	return tex;

}