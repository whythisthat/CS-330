# CS-330

Design Decisions for 3D Scene Creation
This project involved creating a 3D scene in OpenGL. I used low-polygon models to replicate a 2D image with four objects: Tabasco bottle, water bottle, coffee mug, and record drive. Each object was built from simple geometric shapes and enhanced with textures. Lighting and user navigation made the scene more interactive and realistic.

Object Construction
Tabasco Bottle: Cylinders formed the body, and a hexagonal prism shaped the cap. A label texture added realism.
Water Bottle: Built from cylinders. Matte black material gave it an industrial look—no texture needed.
Coffee Mug: Concentric cylinders created the hollow shape, with a torus for the handle. Green color and a smiley face texture added character.
Record Drive: A box with a realistic surface texture. Simple, balancing the scene.
Texture and Lighting
Textures added detail. The Tabasco bottle used a high-resolution label, and the record drive had a realistic surface texture. Mipmaps ensured clear textures across resolutions.

Lighting setup:

Directional Light: Simulated sunlight for consistent lighting.
Point Lights: Highlighted specific areas.
Spotlight: Focused on the scene center to add depth.
Camera and View Controls
Camera controls used WASD for movement, QE for height adjustment, and mouse for orientation. Toggle options between perspective and orthographic views allowed both depth and distortion-free views.

Reflection
How do I approach designing software?
This project enhanced my design skills, particularly in modular construction and balancing functionality with simplicity. I followed an iterative design process, which started with planning object shapes and ended with refining textures and lighting. Going forward, I can apply this structured approach to projects requiring both visual appeal and technical precision.

How do I approach developing programs?
Throughout development, I used modular coding and frequent testing. Each component—object setup, lighting, and camera controls—was built iteratively, allowing me to refine and adjust as needed. My coding approach evolved to prioritize concise, readable functions, and I’ve become more adaptable in applying changes as I work toward final results.


How can computer science help me reach my goals?
Working on computational graphics expanded my technical skills, particularly in object rendering and scene management. These skills are relevant to both educational goals in advanced programming and professional aspirations in fields like game development and UI design. Computational graphics allow me to create visual data representations that are accessible and engaging, which is valuable in both technical and non-technical settings.
