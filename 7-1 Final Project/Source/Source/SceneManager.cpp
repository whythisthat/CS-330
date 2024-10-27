///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}

}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}



/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/***********************************************************
 *  LoadSceneTextures()
 *
 *  This method is used for loading the textures that will
 *  be used for mapping to objects in the 3D scene.
 ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;

	// Load specific textures for objects
	bReturn = CreateGLTexture("textures/wood2.jpg", "floor");
	if (!bReturn) std::cerr << "Error loading texture for 'floor'" << std::endl;

	bReturn = CreateGLTexture("textures/Texturelabs_Glass_134S.jpg", "tbottle");
	if (!bReturn) std::cerr << "Error loading texture for 'tbottle'" << std::endl;

	bReturn = CreateGLTexture("textures/metalbottle.jpg", "bottle");
	if (!bReturn) std::cerr << "Error loading texture for 'bottle'" << std::endl;

	bReturn = CreateGLTexture("textures/tabasco.png", "tlabel");
	if (!bReturn) std::cerr << "Error loading texture for 'tlabel'" << std::endl;

	bReturn = CreateGLTexture("textures/Texturelabs_Paper_334S.jpg", "hdrive");
	if (!bReturn) std::cerr << "Error loading texture for 'hdrive'" << std::endl;

	bReturn = CreateGLTexture("textures/mouth.jpg", "smile");
	if (!bReturn) std::cerr << "Error loading texture for 'smile'" << std::endl;

	bReturn = CreateGLTexture("textures/coffee.png", "coffee");
	if (!bReturn) std::cerr << "Error loading texture for 'coffee'" << std::endl;


	// Bind the textures after loading
	BindGLTextures();
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is used to define and configure the lighting
 *  for the 3D scene using Phong lighting (ambient, diffuse,
 *  and specular components).
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// Enable custom lighting in shaders
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	// Setup directional light (simulating sunlight or uniform light source)
	m_pShaderManager->setVec3Value("directionalLight.direction", -0.05f, -0.3f, -0.1f);  // Direction of light
	m_pShaderManager->setVec3Value("directionalLight.ambient", 0.05f, 0.05f, 0.05f);  // Soft ambient light
	m_pShaderManager->setVec3Value("directionalLight.diffuse", 0.6f, 0.6f, 0.6f);  // Moderate diffuse light
	m_pShaderManager->setVec3Value("directionalLight.specular", 0.0f, 0.0f, 0.0f);  // No specular reflections
	m_pShaderManager->setBoolValue("directionalLight.bActive", true);  // Enable the directional light

	// Setup point light 1
	m_pShaderManager->setVec3Value("pointLights[0].position", -4.0f, 8.0f, 0.0f);  // Position of point light 1
	m_pShaderManager->setVec3Value("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);  // Soft ambient light
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", 0.3f, 0.3f, 0.3f);  // Moderate diffuse light
	m_pShaderManager->setVec3Value("pointLights[0].specular", 0.1f, 0.1f, 0.1f);  // Light specular reflections
	m_pShaderManager->setBoolValue("pointLights[0].bActive", true);  // Enable point light 1

	// Setup point light 2
	m_pShaderManager->setVec3Value("pointLights[1].position", 4.0f, 8.0f, 0.0f);  // Position of point light 2
	m_pShaderManager->setVec3Value("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);  // Soft ambient light
	m_pShaderManager->setVec3Value("pointLights[1].diffuse", 0.3f, 0.3f, 0.3f);  // Moderate diffuse light
	m_pShaderManager->setVec3Value("pointLights[1].specular", 0.1f, 0.1f, 0.1f);  // Light specular reflections
	m_pShaderManager->setBoolValue("pointLights[1].bActive", true);  // Enable point light 2

	// Setup point light 3
	m_pShaderManager->setVec3Value("pointLights[2].position", 3.8f, 5.5f, 4.0f);  // Position of point light 3
	m_pShaderManager->setVec3Value("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);  // Soft ambient light
	m_pShaderManager->setVec3Value("pointLights[2].diffuse", 0.2f, 0.2f, 0.2f);  // Softer diffuse light
	m_pShaderManager->setVec3Value("pointLights[2].specular", 0.8f, 0.8f, 0.8f);  // Strong specular reflections
	m_pShaderManager->setBoolValue("pointLights[2].bActive", true);  // Enable point light 3

	// Setup point light 4
	m_pShaderManager->setVec3Value("pointLights[3].position", 3.8f, 3.5f, 4.0f);  // Position of point light 4
	m_pShaderManager->setVec3Value("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);  // Soft ambient light
	m_pShaderManager->setVec3Value("pointLights[3].diffuse", 0.2f, 0.2f, 0.2f);  // Softer diffuse light
	m_pShaderManager->setVec3Value("pointLights[3].specular", 0.8f, 0.8f, 0.8f);  // Strong specular reflections
	m_pShaderManager->setBoolValue("pointLights[3].bActive", true);  // Enable point light 4

	// Setup point light 5
	m_pShaderManager->setVec3Value("pointLights[4].position", -3.2f, 6.0f, -4.0f);  // Position of point light 5
	m_pShaderManager->setVec3Value("pointLights[4].ambient", 0.05f, 0.05f, 0.05f);  // Soft ambient light
	m_pShaderManager->setVec3Value("pointLights[4].diffuse", 0.9f, 0.9f, 0.9f);  // Brighter diffuse light
	m_pShaderManager->setVec3Value("pointLights[4].specular", 0.1f, 0.1f, 0.1f);  // Light specular reflections
	m_pShaderManager->setBoolValue("pointLights[4].bActive", true);  // Enable point light 5

	// Setup spotlight
	m_pShaderManager->setVec3Value("spotLight.ambient", 0.8f, 0.8f, 0.8f);  // Ambient light for the spotlight
	m_pShaderManager->setVec3Value("spotLight.diffuse", 1.0f, 1.0f, 1.0f);  // Strong diffuse light
	m_pShaderManager->setVec3Value("spotLight.specular", 0.7f, 0.7f, 0.7f);  // Moderate specular reflections
	m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);  // Constant attenuation
	m_pShaderManager->setFloatValue("spotLight.linear", 0.09f);  // Linear attenuation
	m_pShaderManager->setFloatValue("spotLight.quadratic", 0.032f);  // Quadratic attenuation
	m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(42.5f)));  // Spotlight cutoff angle
	m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(48.0f)));  // Outer cutoff for a soft edge
	m_pShaderManager->setBoolValue("spotLight.bActive", true);  // Enable the spotlight

}

void SceneManager::DefineObjectMaterials()
{
	// Define glass material properties
	OBJECT_MATERIAL glassMaterial;
	glassMaterial.diffuseColor = glm::vec3(0.6f, 0.7f, 0.8f);  // Glass slightly tints light passing through
	glassMaterial.specularColor = glm::vec3(0.9f, 0.9f, 1.0f); // Glass is highly reflective with sharp highlights
	glassMaterial.shininess = 85.0f;                           // High shininess for glossy reflections
	glassMaterial.tag = "glass";
	m_objectMaterials.push_back(glassMaterial);

	// Define metal material properties
	OBJECT_MATERIAL metalMaterial;
	metalMaterial.diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);  // Metal reflects light fairly evenly
	metalMaterial.specularColor = glm::vec3(0.8f, 0.8f, 0.8f); // Metal has strong specular reflection
	metalMaterial.shininess = 50.0f;                           // Sharp but not as reflective as glass
	metalMaterial.tag = "metal";
	m_objectMaterials.push_back(metalMaterial);

	// Define wood material properties
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.diffuseColor = glm::vec3(0.6f, 0.4f, 0.2f);  // Wood diffuses light with a soft, warm tone
	woodMaterial.specularColor = glm::vec3(0.2f, 0.1f, 0.1f); // Wood has very little specular reflection
	woodMaterial.shininess = 10.0f;                           // Low shininess for a rough surface
	woodMaterial.tag = "wood";
	m_objectMaterials.push_back(woodMaterial);
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// load the textures for the 3D scene
	LoadSceneTextures();

	DefineObjectMaterials();

	// Setup lighting for the scene
	SetupSceneLights();

	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadConeMesh();      // For the bottle cap
	m_basicMeshes->LoadBoxMesh();       // For the label
	m_basicMeshes->LoadPrismMesh();     // For the hexagonal body (or use a tapered cylinder)
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();

}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	/*
		Table

	*/
	
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(0.4f, 0.2f, 0.1f, 1.0f);
	SetShaderTexture("floor");

	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();
	/****************************************************************/
	/*
		Tabasco Bottle
	
	*/
	// 1. Render the bottom part of the bottle body (Cylinder)
	scaleXYZ = glm::vec3(0.4f, 0.9f, 0.4f);  // Scale for the lower part of the bottle body
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);  // Positioned upright on the ground
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	//SetShaderColor(0.8f, 0.9f, 0.2f, 1.0f);  // Light greenish-yellow color for the bottle body
	SetShaderTexture("tbottle");
	SetShaderMaterial("glass");
	m_basicMeshes->DrawCylinderMesh();  // Draw the lower bottle body (larger cylinder)

	// 2. Render the top part of the bottle body (Cylinder)
	scaleXYZ = glm::vec3(0.25f, 0.6f, 0.25f);  // Scale for the upper part of the bottle body (smaller cylinder)
	positionXYZ = glm::vec3(0.0f, 0.9f, 0.0f);  // Positioned on top of the lower part
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	//SetShaderColor(0.8f, 0.9f, 0.2f, 1.0f);  // Same light greenish-yellow color for the upper bottle body
	SetShaderTexture("tbottle");
	SetShaderMaterial("glass");
	m_basicMeshes->DrawCylinderMesh();  // Draw the upper bottle body (smaller cylinder)

	// 3. Render the hexagonal cap of the bottle (Prism)
	scaleXYZ = glm::vec3(0.3f, 0.3f, 0.3f);  // Scale for the hexagonal cap
	positionXYZ = glm::vec3(0.0f, 1.5f, 0.0f);  // Positioned on top of the upper bottle part
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.9f, 0.1f, 0.1f, 1.0f);  // Red color for the hexagonal cap
	m_basicMeshes->DrawPrismMesh();  // Draw the hexagonal cap

	// 4. Render the label of the bottle (Box or Plane)
	scaleXYZ = glm::vec3(0.2f, 0.45f, 0.025f);  // Thin box for the label
	positionXYZ = glm::vec3(0.0f, 0.5f, 0.4f);  // Positioned in front of the bottle
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	//SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);  // White color for the label
	SetShaderTexture("tlabel");
	m_basicMeshes->DrawBoxMesh();  // Draw the label

	/*
		Water Bottle

	*/


	// Water bottle body (smaller cylinder)
	scaleXYZ = glm::vec3(0.8f, 3.0f, 0.8f);  // Shorter and narrower cylinder for the bottle
	positionXYZ = glm::vec3(-3.0f, 0.0f, 0.0f);  // Positioned to the left of the Tabasco bottle
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.1f, 0.1f, 0.1f, 1.0f);  // Matte black color for the water bottle
	SetShaderMaterial("metal");  // Giving it a metal material for the bottle
	m_basicMeshes->DrawCylinderMesh();  // Draw the smaller bottle body

	// Water bottle bottom ring (smaller thin cylinder)
	scaleXYZ = glm::vec3(0.8f, 0.1f, 0.8f);  // Smaller thin metal ring at the bottom
	positionXYZ = glm::vec3(-3.0f, 0.1f, 0.0f);  // Moved slightly upwards to keep it above the table
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.7f, 0.7f, 0.7f, 1.0f);  // Silver for the bottom ring
	m_basicMeshes->DrawCylinderMesh();  // Draw the smaller ring at the base

	// Water bottle cap (smaller cylinder for the cap)
	scaleXYZ = glm::vec3(0.7f, 0.4f, 0.7f);  // Smaller cap on top of the bottle
	positionXYZ = glm::vec3(-3.0f, 3.0f, 0.0f);  // Positioned on top of the smaller water bottle
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.1f, 0.1f, 0.1f, 1.0f);  // Same black color for the cap
	m_basicMeshes->DrawCylinderMesh();  // Draw the smaller cap

	// Water bottle handle (smaller elongated cylinder)
	scaleXYZ = glm::vec3(0.15f, 0.04f, 0.7f);  // Smaller handle on the side of the cap
	positionXYZ = glm::vec3(-2.6f, 3.2f, 0.0f);  // Positioned on the side of the smaller cap
	SetTransformations(scaleXYZ, 180.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderColor(0.1f, 0.1f, 0.1f, 1.0f);  // Same black color for the handle
	m_basicMeshes->DrawCylinderMesh();  // Draw the smaller handle


	/*
		/This is a record, not a hard drive. I had to switch it because I couldn’t find a suitable image for a hard drive. Finding appropriate images for a hard drive was challenging.


	*/

	// Record drive body (box)
	scaleXYZ = glm::vec3(2.0f, 0.05f, 3.0f);  // Thin and wide box for the hard drive
	positionXYZ = glm::vec3(0.0f, 0.0f, 3.0f);  // Positioned to the right of the Tabasco bottle
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);

	// Set texture wrapping and scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Optionally, set the UV scale to adjust texture size on the object
	SetTextureUVScale(1.0f, 1.0f);  // No scaling

	//SetShaderColor(0.5f, 0.5f, 0.5f, 1.0f);  // Gray color for the hard drive
	SetShaderTexture("hdrive");
	m_basicMeshes->DrawBoxMesh();  // Draw the hard drive body

	/*
		Coffee Mug


	*/


	// Outer mug body (realistic size cylinder)
	scaleXYZ = glm::vec3(0.6f, 0.8f, 0.6f);  // Smaller outer cylinder for the mug body, taller and narrower
	positionXYZ = glm::vec3(3.0f, 0.0f, 0.0f);  // Positioned to the right of the Tabasco bottle
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.0f, 0.8f, 0.0f, 1.0f);  // Green color for the mug
	SetShaderTexture("smile");
	m_basicMeshes->DrawCylinderMesh();  // Draw the outer part of the mug body

	// Inner mug body (realistic hollow cylinder)
	scaleXYZ = glm::vec3(0.59f, 0.79f, 0.59f);  // Slightly smaller inner cylinder to create the hollow effect
	positionXYZ = glm::vec3(3.0f, 0.02f, 0.0f);  // Offset slightly upwards to prevent overlapping with the bottom
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.0f, 0.4f, 0.0f, 1.0f);  // Slightly darker green for the inside of the mug
	SetShaderTexture("coffee");
	m_basicMeshes->DrawCylinderMesh();  // Draw the inner hollow part of the smaller mug body

	// Mug handle (torus)
	scaleXYZ = glm::vec3(0.15f, 0.3f, 0.1f);  // Smaller, proportionate handle for the mug
	positionXYZ = glm::vec3(3.6f, 0.4f, 0.0f);  // Positioned close to the side of the mug body
	SetTransformations(scaleXYZ, 180.0f, 0.0f, 0.0f, positionXYZ);  // Rotate 90 degrees around the X-axis for proper alignment
	SetShaderColor(0.0f, 0.8f, 0.0f, 1.0f);  // Same green color as the mug body
	m_basicMeshes->DrawTorusMesh();  // Draw the smaller handle

	// Flat bottom of the mug (small-height cylinder)
	scaleXYZ = glm::vec3(0.55f, 0.02f, 0.55f);  // Very flat and smaller cylinder for the bottom
	positionXYZ = glm::vec3(3.0f, -0.35f, 0.0f);  // Positioned at the bottom of the mug
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.0f, 0.8f, 0.0f, 1.0f);  // Green color to match the mug
	m_basicMeshes->DrawCylinderMesh();  // Draw the flat smaller bottom

}
