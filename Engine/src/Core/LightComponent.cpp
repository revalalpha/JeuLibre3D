#include "Core/LightComponent.h"
#include <stdexcept>

glm::vec3 PointLightComponent::GetLightColor() const
{
	return m_lightColor;
}

glm::vec3 PointLightComponent::GetSpecularColor() const
{
	return m_specularColor;
}

float PointLightComponent::GetIntensity() const
{
	return m_intensity;
}

float PointLightComponent::GetShiny() const
{
	return m_shiny;
}

void PointLightComponent::SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor)
{
	m_lightColor = lightColor;
	m_specularColor = specularColor;
}

void PointLightComponent::SetParam(float intensity, float shiny)
{
	m_intensity = intensity;
	m_shiny = shiny;
}

LightData PointLightComponent::ToData() const
{
	LightData result;
	result.lightColor = m_lightColor;
	result.specularColor = m_specularColor;
	result.intensity = m_intensity;
	result.shiny = m_shiny;
	result.type = LightData::Point;
	return result;
	
}
glm::vec3 DirectionalLightComponent::GetLightColor() const
{
	return m_lightColor;
}

glm::vec3 DirectionalLightComponent::GetSpecularColor() const
{
	return m_specularColor;
}

float DirectionalLightComponent::GetShiny() const
{
	return m_shiny;
}

void DirectionalLightComponent::SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor)
{
	m_lightColor = lightColor;
	m_specularColor = specularColor;
}

void DirectionalLightComponent::SetParam(float shiny)
{
	m_shiny = shiny;
}

LightData DirectionalLightComponent::ToData() const
{
	LightData result;
	result.lightColor = m_lightColor;
	result.specularColor = m_specularColor;
	result.shiny = m_shiny;
	result.type = LightData::Directional;
	return result;
}

glm::vec3 SpotLightComponent::GetLightColor() const
{
	return m_lightColor;
}

glm::vec3 SpotLightComponent::GetSpecularColor() const
{
	return m_specularColor;
}

float SpotLightComponent::GetIntensity() const
{
	return m_intensity;
}

float SpotLightComponent::GetShiny() const
{
	return m_shiny;
}

float SpotLightComponent::GetOpenAngle() const
{
	return m_openAngle;
}

float SpotLightComponent::GetAdditionalSmooth() const
{
	return m_additionalSmooth;
}

void SpotLightComponent::SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor)
{
	m_lightColor = lightColor;
	m_specularColor = specularColor;
}

void SpotLightComponent::SetParam(float intensity, float shiny)
{
	m_intensity = intensity;
	m_shiny = shiny;
}

void SpotLightComponent::SetAngle(float angle, float additionalSmooth)
{
	m_openAngle = angle;
	m_additionalSmooth = additionalSmooth;
}

LightData SpotLightComponent::ToData() const
{
	LightData result;
	result.lightColor = m_lightColor;
	result.specularColor = m_specularColor;
	result.intensity = m_intensity;
	result.shiny = m_shiny;
	result.openAngle = m_openAngle;
	result.additionalSmooth = m_additionalSmooth;
	result.type = LightData::Spot;
	return result;
}


