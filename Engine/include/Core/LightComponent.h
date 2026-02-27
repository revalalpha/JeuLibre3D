#pragma once

#include <glm/glm.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include "Tools/Util.h"


struct alignas(16) LightData
{
	enum Type
	{
		Point,
		Directional,
		Spot
	};
	PadData<glm::vec3,16> pos = glm::vec3();
	PadData <glm::vec3,16> dir = glm::vec3();
	PadData <glm::vec3,16> lightColor = glm::vec3();
	PadData <glm::vec3,16> specularColor = glm::vec3();
	float intensity = 0.0f;
	float openAngle = 0.0f;
	float additionalSmooth = 0.0f;
	float shiny =0.0f;
	PadData <Type,16> type = LightData::Type::Point;
};

struct PointLightComponent
{
	glm::vec3 GetLightColor() const;
	glm::vec3 GetSpecularColor() const;
	float GetIntensity() const;
	float GetShiny() const;
	void SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor);
	void SetParam(float intensity, float shiny);
	LightData ToData() const;
private :
	glm::vec3 m_lightColor = glm::vec3{ 1.0f,1.0f,1.0f };
	glm::vec3 m_specularColor = glm::vec3{ 1.0f,1.0f,1.0f };
	float m_intensity = 100.0f;
	float m_shiny = 100.0f;
};

struct DirectionalLightComponent
{
	glm::vec3 GetLightColor() const;
	glm::vec3 GetSpecularColor() const;
	float GetShiny() const;
	void SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor);
	void SetParam(float shiny);
	LightData ToData() const;
private:
	glm::vec3 m_lightColor = glm::vec3{ 1.0f,1.0f,1.0f };
	glm::vec3 m_specularColor = glm::vec3{ 1.0f,1.0f,1.0f };
	float m_shiny = 100.0f;
};

struct SpotLightComponent
{
	glm::vec3 GetLightColor() const;
	glm::vec3 GetSpecularColor() const;
	float GetIntensity() const;
	float GetShiny() const;
	float GetOpenAngle() const;
	float GetAdditionalSmooth() const;
	void SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor);
	void SetParam(float intensity, float shiny);
	void SetAngle(float angle, float additionalSmooth);
	LightData ToData() const;
private:
	glm::vec3 m_lightColor = glm::vec3{ 1.0f,1.0f,1.0f };
	glm::vec3 m_specularColor = glm::vec3{ 1.0f,1.0f,1.0f };
	float m_intensity = 100.0f;
	float m_shiny = 100.0f;
	float m_openAngle = glm::radians(45.0f);
	float m_additionalSmooth = 5.0f;
};


