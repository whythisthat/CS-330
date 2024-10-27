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
I start with planning and iterate as I go. This project built skills in modular design and practical problem-solving. I used a structured approach, focusing on each object’s shape and refining textures and lighting last. I’ll apply this iterative method to future projects that need a balance of visual detail and technical function.

How do I approach developing programs?
I focus on modular code and test frequently. I broke down each part—object setup, lighting, and controls—building and adjusting in stages. My coding approach is now more concise, with an emphasis on readability and adaptability.

How can computer science help me reach my goals?
Computational graphics gives me new skills in scene management and data visualization. These skills align with my educational goals in programming and future roles in areas like game development and UI design. Creating engaging visuals adds value to technical and non-technical projects alike.

