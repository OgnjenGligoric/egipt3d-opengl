#pragma once
#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"
#include "shader.h"


class ModelRenderer
{
public:
	float CameraAngleX = 45.0f;
	float CameraAngleY = 0.0f;
	float CameraPositionX = 0.0f;
	float CameraPositionY = 0.0f;
	float CameraPositionZ = 0.0f;
	glm::vec3 Position;
	glm::vec3 Size;
	glm::vec3 Rotation;
	ModelRenderer(Shader& shader, Model& model, glm::vec3 position, glm::vec3 size = glm::vec3(10.0f, 10.0f, 10.0f),
	              glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	~ModelRenderer();
	void DrawModel();

private:
	Shader shader;
	Model& model;
};

#endif
