#pragma once   //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "bezier.h"
#include <iostream>
bool leftPressedInside = false;
bool rightPressedInside = false;
bool leftPressedEdges = false;
bool rightPressedEdges = false;
int globalID = 22;
void HandleInsidePoints(Renderer* rndr, bezier* scn, int button, double x , double y)
{
	scn->updatePressedPos(x, y);
	rndr->UpdatePosition(x, y);
	switch (button)
	{
	case  GLFW_MOUSE_BUTTON_RIGHT : 
		if (!rightPressedInside)
		{
			rightPressedInside = !rightPressedInside;
		}
		break;
	case  GLFW_MOUSE_BUTTON_LEFT:
		if (!leftPressedInside)
		{
			leftPressedInside = !leftPressedInside;
		}
		break;
	default:
		break;
	}
	
}

void HandleEdgesPoints(Renderer* rndr, bezier* scn, int button, double x, double y)
{
	scn->updatePressedPos(x, y);
	rndr->UpdatePosition(x, y);
	scn->AlignPoints();
	switch (button)
	{
	case  GLFW_MOUSE_BUTTON_RIGHT:
		if (!rightPressedEdges)
		{
			rightPressedEdges = true;
			
		}
		break;
	case  GLFW_MOUSE_BUTTON_LEFT:
		if (!leftPressedEdges)
		{
			leftPressedEdges = true;
		}
		
		break;
	default:
		break;
	}

}
	void mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		bezier* scn = (bezier*)rndr->GetScene();
		if (action == GLFW_PRESS)
		{
			double x2, y2;
			glfwGetCursorPos(window, &x2, &y2);
			if (rndr->Picking((int)x2, (int)y2))
			{
				std::cout << "picked: " << scn->GetPickedShape() << std::endl;
				if (scn->GetPickedShape() > 1 && scn->GetPickedShape() <= 21) {
					int id = (scn->GetPickedShape() - 2) % 3;
					if (id != 0) {
						HandleInsidePoints(rndr, scn, button , x2  , y2);
					}
					else
					{
						HandleEdgesPoints(rndr, scn, button, x2, y2);
					}
				}
			}
		}
	}
	void reDrawAll(int points, Scene* scn) {
		float PI = 3.141592654;
		for (size_t i = 0; i < points; i++)
		{
			scn->RemoveShapeViewport(i + 2, 1);
			scn->ZeroShapeTrans(i + 2);
			scn->SetShapeShader(i + 2, 1);
			float angle = PI * i / points;
			float xTrans = (((float)-points / 2.f) + i) / 8.8f;
			float yTrans = i == (points - 1) || i == 0 ? 0 : (2.f + 3.f * sin(angle)) / 8.8f;
			scn->SetPickedShape(i + 2);
			scn->ShapeTransformation(scn->xTranslate, xTrans);
			scn->ShapeTransformation(scn->yTranslate, yTrans);
			scn->GetBezier1D()->GetControlPoints().push_back(glm::vec3(xTrans, yTrans, 0.f));
			scn->SetPickedShape(-1);
			scn->AddShapeViewport(i + 2, 1);
		}
	}
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		double xpos;
		double ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		bezier* scn = (bezier*)rndr->GetScene();
		std::cout << " x " << xpos << std::endl;
		if (xpos < 600) {
			if (yoffset > 0) {
				rndr->MoveCamera(0, scn->zTranslate, 0.4f);
			}
			else {
				rndr->MoveCamera(0, scn->zTranslate, -0.4f);
			}
		}
		else {
			if (yoffset > 0) {
				rndr->MoveCamera(1, scn->zTranslate, 0.4f);
			}
			else {
				rndr->MoveCamera(1, scn->zTranslate, -0.4f);
			}
		}
		
		
	}
	
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		bezier* scn = (bezier*)rndr->GetScene();
		
			rndr->UpdatePosition((float)xpos,(float)ypos);

			if (rightPressedInside) {
				scn->setNewOffset((float)xpos, (float)ypos);
			}

			if (leftPressedInside) {
				scn->setNewOffsetWithRotate((float)xpos, (float)ypos);
			}

			if (rightPressedEdges) {
				scn->setNewOffsetWithChilds((float)xpos, (float)ypos);
			}

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
			{
				if (rightPressedInside) {
					rightPressedInside = !rightPressedInside;
					scn->setNewOffset(xpos, ypos);
					rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
				}
				if (leftPressedInside) {
					leftPressedInside = !leftPressedInside;
					scn->setNewOffsetWithRotate(xpos, ypos);
					rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
				}
				if (rightPressedEdges) {
					rightPressedEdges = !rightPressedEdges;
					scn->setNewOffsetWithChilds(xpos, ypos);
					rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
				}
				if (leftPressedEdges) {
					leftPressedEdges = !leftPressedEdges;
					rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
				}
			}

	}

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		rndr->Resize(width,height);
		
	}
	
	void Bezier1DPoints(Scene* scn, int seg, int diffSeg, bool add)
	{
		int points;

		int oldSize = scn->GetBezier1D()->GetControlPoints().size();

		if (add)
		{
			points = oldSize + (3 * diffSeg);
		}
		else
		{
			points = oldSize - (3 * diffSeg);
		}

		float PI = 3.141592654;

		for (size_t i = 0; i < oldSize; i++)
		{
			scn->RemoveShapeViewport(i + 2, 1);
		}

		scn->GetBezier1D()->GetControlPoints().clear();
		reDrawAll(points, scn);
		scn->GetBezier1D()->AddSegment(seg);
	}

	void darwNewBezier1D(Scene* scn, int seg)
	{
		int oldSeg = scn->GetBezier1D()->GetSegmentsNum();

		if (oldSeg > seg)
		{
			Bezier1DPoints(scn, seg, oldSeg - seg, false);
		}
		else if (oldSeg < seg)
		{
			Bezier1DPoints(scn, seg, seg - oldSeg, true);
		}
	}
	
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		bezier* scn = (bezier*)rndr->GetScene();

		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case GLFW_KEY_SPACE:
				scn->AddShape(scn->GetBezier1D()->GetSegmentsNum(), scn->GetBezier1D()->GetControlPoints(), -1, scn->TRIANGLES); // Add Bezier2D To Scene
				scn->RemoveShapeViewport(globalID, 1);
				scn->AddShapeViewport(globalID, 0);
				scn->SetShapeShader(globalID, 1);
				scn->SetShapeMaterial(globalID++, 1);
				scn->SetPickedShape(globalID - 1);
				scn->ShapeTransformation(scn->xTranslate, 0.3f);
				scn->SetPickedShape(-1);
				break;
			case GLFW_KEY_2:
				darwNewBezier1D(scn, 2);
				break;
			case GLFW_KEY_3:
				darwNewBezier1D(scn, 3);
				break;
			case GLFW_KEY_4:
				darwNewBezier1D(scn, 4);
				break;
			case GLFW_KEY_5:
				darwNewBezier1D(scn, 5);
				break;
			case GLFW_KEY_6:
				darwNewBezier1D(scn, 6);
				break;
			case GLFW_KEY_LEFT:
				rndr->MoveCamera(0, rndr->yRotate, -0.8f);
				break;
			case GLFW_KEY_RIGHT:
				rndr->MoveCamera(0, rndr->yRotate, 0.8f);
				break;
			case GLFW_KEY_UP:
				rndr->MoveCamera(0, rndr->xRotate, 0.8);
				break;
			case GLFW_KEY_DOWN:
				rndr->MoveCamera(0, rndr->xRotate, -0.8);
			case GLFW_KEY_R:
				rndr->MoveCamera(0, rndr->yRotate, 0.8);
				break;
			case GLFW_KEY_L:
				rndr->MoveCamera(0, rndr->yRotate, -0.8);
				break;
			case GLFW_KEY_D:
				rndr->MoveCamera(0, rndr->xRotate, -0.8);
				break;
			case GLFW_KEY_U:
				rndr->MoveCamera(0, rndr->xRotate, 0.8);
				break;
			case GLFW_KEY_B:
				rndr->MoveCamera(0, rndr->zTranslate, 0.1);
				break;
			case GLFW_KEY_F:
				rndr->MoveCamera(0, rndr->zTranslate, -0.1);
				break;
			default:
				break;
			}
		}
	}

	void Init(Display &display)
	{
		display.AddKeyCallBack(key_callback);
		display.AddMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
		display.AddResizeCallBack(window_size_callback);
	}

	

	


