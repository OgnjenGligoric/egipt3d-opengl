#include "model_renderer.h"

ModelRenderer::ModelRenderer(Shader& shader, Model& model, glm::vec3 position, glm::vec3 size, glm::vec3 rotation):
	Position(position),
	Size(size),
	Rotation(rotation),
	shader(shader),
	model(model) {}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::DrawModel()
{
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
	view = glm::rotate(view, glm::radians(CameraAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, glm::radians(CameraAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::translate(view, glm::vec3(CameraPositionX, CameraPositionY, CameraPositionZ));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Position); // translate it down so it's at the center of the scene
	model = glm::scale(model, Size);
	shader.Use().SetMatrix4("model", model);
	shader.Use().SetMatrix4("view", view);

	this->model.Draw(shader.Use());
}

