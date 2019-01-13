function [ ...
	vertexCount, faceCount, tetraCount, ...
	vertexList, tetraVertexIndex, faceVertexIndex, ...
	tetraFaceIndex, faceTetraIndex ...
] = GenerateMesh_SingleTet()

	v0 = [ 0; 0; 0 ];
	vx = [ 1; 0; 0 ];
	vy = [ 0; 1; 0 ];
	vz = [ 0; 0; 1 ];

	vertexCount = 4;
	vertexList = [ v0, vx, vy, vz ];

	tetraCount = 1;
	tetraVertexIndex = ...
		[1;2;3;4] ...
	;

	faceCount = 4;
	faceVertexIndex = [ ...
		[1;2;3], ...
		[1;2;4], ...
		[1;3;4], ...
		[2;3;4] ...
	];

	tetraFaceIndex = ...
		[1;2;3;4] ...
	;

	faceTetraIndex = [ ...
		[1;-1], ...
		[1;-1], ...
		[1;-1], ...
		[1;-1] ...
	];
end
