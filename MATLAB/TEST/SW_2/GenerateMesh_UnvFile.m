function [ ...
	vertexCount, faceCount, tetraCount, ...
	vertexList, tetraVertexIndex, faceVertexIndex, ...
	tetraFaceIndex, faceTetraIndex ...
] = GenerateMesh_UnvFile( fileName )
	
	run( "..\Misc\AddPath_" );
	p = load( "..\Misc\pathList.mat" );
	
	MakeMesh( fileName );
	MakeFace( fileName );
	
	[ vertexCount, tetraCount, vertexList, tetraVertexIndex ] = LoadTetraMesh( fileName );
	[ faceCount, tetraCount, faceVertexIndex, faceTetraIndex, tetraFaceIndex ] = LoadTetraFace( fileName );
	
	vertexList = vertexList';
	tetraVertexIndex = tetraVertexIndex' + 1;
	faceVertexIndex = faceVertexIndex' + 1;
	
	faceTetraIndex( faceTetraIndex > 1000000 ) = 1000000;
	faceTetraIndex = faceTetraIndex' + 1;
	faceTetraIndex( faceTetraIndex == 1000001 ) = -1;
	
	tetraFaceIndex = tetraFaceIndex' + 1;
	
	run( "..\Misc\RmPath_" );
	
end

