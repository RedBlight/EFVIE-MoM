q1 = [3;5];
q2 = [10;100];
q3 = [11;101];

[ q1, q2, q3 ] = meshgrid( q1, q2, q3 );

q1 = q1(:);
q2 = q2(:);
q3 = q3(:);

q1
q2
q3

% ww = reshape( reshape( q2 * q1', 2^2, 1 ) * q3', 2^3, 1 );
% 
% ww