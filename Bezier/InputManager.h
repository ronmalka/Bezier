#pragma once   //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "bezier.h"
#include <iostream>
float addition = 0.f;

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

				rndr->UpdatePosition(x2, y2);
			}
			scn->ResetCounter();
		}
		else
			scn->SetCounter();
		//std::cout << "yes" << std::endl;
		//rndr->ClearDrawFlag(1,1);
	}
	void reDrawAll(int points, Scene* scn) {
		float PI = 3.141592654;
		for (size_t i = 0; i < points; i++)
		{
			scn->ZeroShapeTrans(i + 2);
			scn->SetPickedShape(i + 2);
			float angle = PI * i / points;
			float xTrans = (-(float)points / 2 + i) / 8.f ;
			float yTrans = (i == points - 1 || i == 0 ? 0.f : (2.f * sin(angle)) / 8.f) + addition ;
			std::cout << "xtrans : " << xTrans << " ytrans : " << yTrans << std::endl;
			scn->ShapeTransformation(scn->xTranslate, xTrans);
			scn->ShapeTransformation(scn->yTranslate, yTrans);
			scn->GetBezier1D()->GetControlPoints().push_back(glm::vec3(xTrans, yTrans, scn->GetPickedShape()));
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
		if (xpos < 800) {
			if (yoffset > 0) {
				rndr->MoveCamera(0, scn->zTranslate, 0.4f);
			}
			else {
				rndr->MoveCamera(0, scn->zTranslate, -0.4f);
			}
		}
		else {
			if (yoffset > 0) {
				addition += 0.2f;
				reDrawAll(scn->GetBezier1D()->GetControlPoints().size(),scn);
			}
			else {
				addition -= 0.2f;
				reDrawAll(scn->GetBezier1D()->GetControlPoints().size(),scn);
			}
		}
		
		
	}
	
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		bezier* scn = (bezier*)rndr->GetScene();
		
		rndr->UpdatePosition((float)xpos,(float)ypos);

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				//scn->UpdatePosition((float)xpos, (float)ypos);
				//rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
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
				if (scn->IsActive())
					scn->Deactivate();
				else
					scn->Activate();
				break;
			case GLFW_KEY_LEFT:
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

	

	


