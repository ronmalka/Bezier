#pragma once   //maybe should be static class
#define MY_PLANE 22
#include "display.h"
#include "renderer.h"
#include "bezier.h"
#include <iostream>
#include <set>
bool leftPressedInside = false;
bool rightPressedInside = false;
bool leftPressedEdges = false;
bool rightPressedEdges = false;
bool threeDPressed = false;
bool isRotate = false;
bool movepickeds = false;
float zoomCo = 1.f;
float twoDzoom = 0.f;
float threeDzoom = 0.f;
int globalID = 23;

void movePlane(Scene* scn,int action,float amt) {
	scn->SetPickedShape(MY_PLANE);
	scn->ShapeTransformation(action, amt); // Move Plane
	scn->SetPickedShape(-1);
}

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
			scn->setAngel(true);
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
		scn->picked.clear();
		movepickeds = false;
		if (action == GLFW_PRESS)
		{
			double x2, y2;
			glfwGetCursorPos(window, &x2, &y2);
			if (rndr->Picking((int)x2, (int)y2))
			{
				if (scn->GetPickedShape() > 1 && scn->GetPickedShape() < 21) { //Control Points
					int id = (scn->GetPickedShape() - 2) % 3;
					if (id != 0) {
						HandleInsidePoints(rndr, scn, button , x2  , y2);
					}
					else
					{
						HandleEdgesPoints(rndr, scn, button, x2, y2);
					}
				}
				else if (scn->GetPickedShape() >= 23 && scn->GetPickedShape() <=globalID) { //Shapes in 3D
					scn->updatePressedPos(x2, y2);
					threeDPressed = true;
					if (button == GLFW_MOUSE_BUTTON_LEFT) { isRotate = true; } 
					else isRotate = false;
				}
				else {
					//Blending
					if (button == GLFW_MOUSE_BUTTON_LEFT && x2 < 600) {
						rndr->whenBlend(x2,y2);
						rndr->isClicked = rndr->inAction2;
					}
				}
			}
			else if (x2 > 600 && button == GLFW_MOUSE_BUTTON_LEFT)
			{
				scn->GetBezier1D()->GetControlPointsWorld().clear();
				for (int i = 0; i < scn->GetBezier1D()->GetControlPoints().size(); i++) {

					glm::vec4 pos = scn->shapes[i+2]->MakeTrans() * glm::vec4(0.f, 0.f, 0.f, 1.f);
					glm::vec3 win_cor = rndr->DoProject(glm::vec3(pos.x, pos.y, pos.z),1,1);
					scn->GetBezier1D()->GetControlPointsWorld().push_back(win_cor);

				}

				scn->HandleConvexHull(x2, 600 - y2);
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
		if (xpos < 600) {
			if (yoffset > 0) {
				if (zoomCo < 10.2f) {
					rndr->MoveCamera(0, rndr->zTranslate, 0.4f);
					movePlane(scn, scn->zTranslate, 0.4f);
					zoomCo += 0.2f;
				}
			}
			else {
				if (zoomCo > -8.2f) {
					rndr->MoveCamera(0, scn->zTranslate, -0.4f);
					movePlane(scn, scn->zTranslate, -0.4f);
					zoomCo -= 0.2f;
				}
			}
		}
		else {
			if (yoffset > 0) {
				rndr->MoveCamera(1, scn->zTranslate, 0.2f);
				twoDzoom += 0.2f;
			}
			else {
				if (twoDzoom > -1.6) {
					rndr->MoveCamera(1, scn->zTranslate, -0.2f);
					twoDzoom -= 0.2f;
				}
			}
		}
		
		
	}
	
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		bezier* scn = (bezier*)rndr->GetScene();
		rndr->UpdatePosition((float)xpos,(float)ypos);
		scn->xPos = (int)xpos;
		scn->yPos = (int)ypos;

			if (rightPressedInside) {
				scn->setNewOffset((float)xpos, (float)ypos,false,false, zoomCo);
			}

			if (leftPressedInside) {
				scn->setNewOffsetWithRotate((float)xpos, (float)ypos);
			}

			if (rightPressedEdges) {
				scn->setNewOffsetWithChilds((float)xpos, (float)ypos);
			}
			if (threeDPressed) {
				scn->setNewOffset((float)xpos, (float)ypos,true,isRotate,zoomCo);
			}

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				scn->picked.clear();
				movepickeds = false;
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				scn->picked.clear();
				movepickeds = false;
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
			{
				if (rndr->isClicked) {
					rndr->isClicked = 0;
					scn->picked.clear();
					int x = glm::min(rndr->xWhenBlend, (float)xpos); //x is xWhenPressed
					int y = glm::min(600.f - rndr->yWhenBlend, 600.f - (float)ypos); //y is yWhenPressed
					int width = glm::abs((int)rndr->xWhenBlend - xpos);
					int hight = glm::abs((int)rndr->yWhenBlend - ypos);
					for (int i = 0; i < (globalID - 23); i++) {
						glm::vec4 pos = scn->shapes[23 + i]->MakeTrans() * glm::vec4(0.f,0.f,0.f,1.f);
						glm::vec3 win_cor = rndr->DoProject(glm::vec3(pos.x,pos.y,pos.z),0,0);
						if (win_cor.x >= x && win_cor.x <= x+width && win_cor.y >= y && win_cor.y <= y+width) {
							scn->picked.insert(23+i);
						}
					}
					//scn->updatePickedShapes(from,to);
					movepickeds = true;
					scn->updatePressedPos(xpos, ypos);

				}
				if (movepickeds) {
					scn->pickedMove(xpos, ypos, zoomCo);
				}
				if (rightPressedInside) {
					rightPressedInside = !rightPressedInside;
					scn->setNewOffset(xpos, ypos,false,false,zoomCo);
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
					//rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
				}
				if (threeDPressed) {
					threeDPressed = !threeDPressed;
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
			{
				scn->AddShape(scn->GetBezier1D()->GetSegmentsNum(), scn->GetBezier1D()->GetControlPoints(), -1, scn->TRIANGLES); // Add Bezier2D To Scene
				scn->RemoveShapeViewport(globalID, 1);
				scn->AddShapeViewport(globalID, 0);
				scn->SetShapeShader(globalID, 5);
				scn->SetPickedShape(globalID);
				scn->ShapeTransformation(scn->zRotate,-90.f);
				scn->SetShapeMaterial(globalID++, 1);
				break;
			}
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
				rndr->MoveCamera(0, rndr->zTranslate, -10.0f);
				movePlane(scn, scn->zTranslate, -10.0f);
				rndr->MoveCamera(0, rndr->yRotate, 0.8f);
				movePlane(scn, scn->yRotate, 0.8f);
				rndr->MoveCamera(0, rndr->zTranslate, 10.0f);
				movePlane(scn, scn->zTranslate, 10.0f);
				break;
			case GLFW_KEY_RIGHT:
				rndr->MoveCamera(0, rndr->zTranslate, -10.0f);
				movePlane(scn, scn->zTranslate, -10.0f);
				rndr->MoveCamera(0, rndr->yRotate, -0.8f);
				movePlane(scn, scn->yRotate, -0.8f);
				rndr->MoveCamera(0, rndr->zTranslate, 10.0f);
				movePlane(scn, scn->zTranslate, 10.0f);
				break;
			case GLFW_KEY_UP:
				rndr->MoveCamera(0, rndr->zTranslate, -10.0f);
				rndr->MoveCamera(0, rndr->xRotate, 0.8);
				rndr->MoveCamera(0, rndr->zTranslate, 10.0f);
				movePlane(scn, scn->zTranslate, -10.0f);
				movePlane(scn, scn->xRotate, 0.8f);
				movePlane(scn, scn->zTranslate, 10.0f);
				break;
			case GLFW_KEY_DOWN:
				rndr->MoveCamera(0, rndr->zTranslate, -10.0f);
				rndr->MoveCamera(0, rndr->xRotate, -0.8);
				rndr->MoveCamera(0, rndr->zTranslate, 10.0f);
				movePlane(scn, scn->zTranslate, -10.0f);
				movePlane(scn, scn->xRotate, -0.8f);
				movePlane(scn, scn->zTranslate, 10.0f);
				break;
			case GLFW_KEY_R:
				rndr->MoveCamera(0, rndr->xTranslate, 0.8);
				movePlane(scn, scn->xTranslate, 0.8f);
				break;
			case GLFW_KEY_L:
				rndr->MoveCamera(0, rndr->xTranslate, -0.8);
				movePlane(scn, scn->xTranslate, -0.8f);
				break;
			case GLFW_KEY_D:
				rndr->MoveCamera(0, rndr->yTranslate, -0.8);
				movePlane(scn, scn->yTranslate, -0.8f);
				break;
			case GLFW_KEY_U:
				rndr->MoveCamera(0, rndr->yTranslate, 0.8);
				movePlane(scn, scn->yTranslate, 0.8f);
				break;
			case GLFW_KEY_B:
				rndr->MoveCamera(0, rndr->zTranslate, 0.1);
				movePlane(scn, scn->zTranslate, 0.1f);
				break;
			case GLFW_KEY_F:
				rndr->MoveCamera(0, rndr->zTranslate, -0.1);
				movePlane(scn, scn->zTranslate, -0.1f);
				break;
			case GLFW_KEY_C:
				scn->setCont();
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

	

	


