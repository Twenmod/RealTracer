#pragma once
class Texture
{
public:
	virtual ~Texture() = default;

	virtual ColorGroup Sample(xs::batch<float> u, xs::batch<float> v, const Point3Group& point) const = 0;
private:
};

class SolidColorTexture : public Texture
{
public:
	SolidColorTexture(ColorGroup albedo);

	ColorGroup Sample(xs::batch<float> u, xs::batch<float> v, const Point3Group& point) const override;

private:
	ColorGroup m_albedo;
};

class CheckerTexture3D : public Texture
{
public:
	CheckerTexture3D(float scale, ColorGroup color1, ColorGroup color2);
	ColorGroup Sample(xs::batch<float> u, xs::batch<float> v, const Point3Group& point) const override;

private:
	float m_invscale;
	ColorGroup m_color1;
	ColorGroup m_color2;
};

class ImageTexture : public Texture
{
public:
	ImageTexture(const char* file, bool _flip = false);
	ColorGroup Sample(xs::batch<float> u, xs::batch<float> v, const Point3Group& point) const override;
private:
	std::vector<float> m_r;
	std::vector<float> m_g;
	std::vector<float> m_b;
	int m_width, m_height;
};