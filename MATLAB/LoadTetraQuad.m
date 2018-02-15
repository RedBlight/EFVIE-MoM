function [ tetraCount, quadCount, quadData ] = LoadTetraQuad( fileName )
    
    fileID = fopen( "Data/" + fileName + ".tetraquad" );
    
    tetraCount = fread( fileID, 1, 'uint64' );
    
    quadCount = fread( fileID, 1, 'uint64' );
    
    quadData = fread( fileID, tetraCount * quadCount * 4, 'double' );
    quadData = transpose( reshape( quadData, [ 4, tetraCount * quadCount ] ) );
    
    fclose( fileID );
end

