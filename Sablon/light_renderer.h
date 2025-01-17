#pragma once
#ifndef LIGHT_RENDERER_H
#define LIGHT_RENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"
#include "shader.h"


class LightRenderer
{
public:
	glm::vec3 Position;
	glm::vec3 Size;
	glm::vec3 Rotation;
	LightRenderer(Shader& shader, glm::vec3 position, glm::vec3 size = glm::vec3(10.0f, 10.0f, 10.0f),
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f)) :
		Position(position),
		Size(size),
		Rotation(rotation),
		shader(shader) {}
	LightRenderer() {}
	~LightRenderer();
	void DrawLight(Model& model);

private:
	Shader shader;
};

#endif
void LightRenderer::DrawLight(Model& model)
{
	glBindTexture(GL_TEXTURE_2D, 0);

	glm::mat4 model_shader = glm::mat4(1.0f);
	model_shader = glm::translate(model_shader, Position); // translate it down so it's at the center of the scene
	model_shader = glm::scale(model_shader, Size);
	model_shader = glm::rotate(model_shader, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model_shader = glm::rotate(model_shader, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model_shader = glm::rotate(model_shader, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	shader.Use().SetMatrix4("model", model_shader);
	model.Draw(shader);
}

LightRenderer::~LightRenderer()
{
}