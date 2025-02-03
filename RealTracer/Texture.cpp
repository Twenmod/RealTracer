#include "precomp.h"
#include "Common.h"
#include "Texture.h"

SolidColorTexture::SolidColorTexture(ColorGroup _albedo)
{
	m_albedo = _albedo;
}

ColorGroup SolidColorTexture::Sample(xs::batch<float> u, xs::batch<float> v, const Point3Group&) const
{
	return m_albedo;
}

CheckerTexture3D::CheckerTexture3D(float _scale, ColorGroup _color1, ColorGroup _color2) :
	m_invscale(1.f/_scale),
	m_color1(_color1),
	m_color2(_color2)
{
}

ColorGroup CheckerTexture3D::Sample(xs::batch<float> _u, xs::batch<float> _v, const Point3Group& _point) const
{
	xs::batch<int> x = xs::batch_cast<int>(floor(_point.x*m_invscale));
	xs::batch<int> y = xs::batch_cast<int>(floor(_point.y*m_invscale));
	xs::batch<int> z = xs::batch_cast<int>(floor(_point.z*m_invscale));
	xs::batch_bool<float> isEven = xs::batch_bool_cast<float>((x + y + z) % 2 == 0);
	ColorGroup color;
	color.x = xs::select(isEven, m_color1.x, m_color2.x);
	color.y = xs::select(isEven, m_color1.y, m_color2.y);
	color.z = xs::select(isEven, m_color1.z, m_color2.z);
	return color;
}

ImageTexture::ImageTexture(const char* _file, bool _flip)
{
	stbi_set_flip_vertically_on_load(_flip);

	int nrChannels;
	unsigned char* data = stbi_load(_file, &m_width, &m_height, &nrChannels, 0);
	if (data)
	{
		m_r.resize(m_width * m_height);
		m_g.resize(m_width * m_height);
		m_b.resize(m_width * m_height);
		for (size_t i = 0; i < m_width * m_height; i++)
		{
			m_r[i] = data[i * nrChannels] / static_cast<float>(0xff);
			m_g[i] = data[i * nrChannels + 1] / static_cast<float>(0xff);
			m_b[i] = data[i * nrChannels + 2] / static_cast<float>(0xff);
		}
	}
	else
	{
		Logger::LogWarning((std::string("FAILED TO LOAD TEXTURE AT: ") + _file).c_str(), WARNING_SEVERITY::MEDIUM);
	}

}

ColorGroup ImageTexture::Sample(xs::batch<float> u, xs::batch<float> v, const Point3Group& point) const
{

	if (m_width <= 0 || m_height <= 0) return ColorGroup(1, 0, 1);

	xs::batch_bool<float> isNan = xs::isnan(u);
	u = max(min(xs::select(isNan, xs::batch<float>(0), u),xs::batch<float>(1.f)), xs::batch<float>(0));
	isNan = xs::isnan(v);
	v = 1.f - max(min(xs::select(isNan, xs::batch<float>(0), v), xs::batch<float>(1.f)), xs::batch<float>(0));

	xs::batch<int> x = xs::batch_cast<int>(xs::round(u * (m_width - 1)));
	xs::batch<int> y = xs::batch_cast<int>(xs::round(v * (m_height - 1)));
	xs::batch<int> pixelIndex = x + y * m_width;

	ColorGroup color;
	color.x = xs::batch<float>::gather(&m_r[0], pixelIndex);
	color.y = xs::batch<float>::gather(&m_g[0], pixelIndex);
	color.z = xs::batch<float>::gather(&m_b[0], pixelIndex);

	return color;
}
