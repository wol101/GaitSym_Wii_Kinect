% function to wrap a line around a cylinder

% the cylinder is assumed to have its axis along the z axis and the wrapping is according
% to the right hand rule

% the coordinate system is right handed too

function [wrapOK, originForce, insertionForce, cylinderForce, cylinderForcePosition, pathLength, pathCoordinates] = ...
   CylinderWrap(origin, insertion, radius, tension, nExtraPathCoordinates, maxAngle)

% first of all calculate the planar case looking down the axis of the cylinder (i.e. xy plane)
% this is standard tangent to a circle stuff


% Let's define some variables:
% originX: the x coordinate of the origin
% originY: the y coordinate of the origin
% d1: the distance of origin from the centre
% l1: the distance of the origin from the circle along its tangent
% theta1: the angle of the line from the centre to the origin
% phi1:the angle at the centre formed by the origin triangle
% r: the radius of the circle

originX = origin(1);
originY = origin(2);
originZ = origin(3);
insertionX = insertion(1);
insertionY = insertion(2);
insertionZ = insertion(3);
r = radius;

d1 = sqrt(originX^2 + originY^2);
% error condition
if (d1 <= radius)
   wrapOK = -1
   return
end

theta1=atan2(originY,originX);

phi1=acos(r/d1);

l1=d1 * sin(phi1);

% The spherical coordinates of the tangent point are (r, theta1+phi1)

tangentPointTheta1=theta1+phi1;

tangentPointX1=r * cos(tangentPointTheta1);

tangentPointY1=r * sin(tangentPointTheta1);

% More variables:
% insertionX: the x coordinate of the insertion
% insertionY: the y coordinate of the inserttion
% d2:the distance of the insertion from the centre
% l2:the distance of the insertion from the centre
% theta2:the angle of the line from the centre to the insertion
% phi2:the angle at the centre formed by the insertion triangle

d2 = sqrt(insertionX^2 + insertionY^2);
% error condition
if (d2 <= radius)
   wrapOK = -1
   return
end

theta2=atan2(insertionY,insertionX);

phi2=acos(r/d2);

l2=d2 * sin(phi2);

% The spherical coordinates of the tangent point are (r, theta2-phi2)

tangentPointTheta2=theta2-phi2;

tangentPointX2=r * cos(tangentPointTheta2);

tangentPointY2=r * sin(tangentPointTheta2);

% rho: the angle around the circumference of the path in this plane
% c:the distance around the circumference of the path in this plane
rho=tangentPointTheta2-tangentPointTheta1;
while (rho < 0)
   rho = rho + 2 * pi;
end
while (rho > 2 * pi)
   rho = rho - 2 * pi;
end

c = r * rho;

% Finally we need to decide whether the path wraps at all. 
% We do this by seeing if the wrap angle is greater than a user specified limit.
% Useful limits will be between pi and 2 pi

if (rho > maxAngle)
   wrapOK = 0
   
 	% now calculate some forces

	originForceX = insertionX - originX;
	originForceY = insertionY - originY;
	originForceZ = insertionZ - originZ;
	l = sqrt(originForceX^2 + originForceY^2 + originForceZ^2);
	originForce(1) = originForceX * tension / l;
	originForce(2) = originForceY * tension / l;
	originForce(3) = originForceZ * tension / l;

	insertionForce(1) = -originForce(1);
	insertionForce(2) = -originForce(2);
	insertionForce(3) = -originForce(3);

	cylinderForce(1) = 0;
	cylinderForce(2) = 0;
	cylinderForce(3) = 0;

	cylinderForcePosition(1) = 0;
	cylinderForcePosition(2) = 0;
	cylinderForcePosition(3) = 0;
   
   pathLength = sqrt((insertion(1) - origin(1))^2 + (insertion(2) - origin(2))^2 + (insertion(3) - origin(3))^2);
   
   % and a simple straight path
   pathCoordinates = zeros( 2, 3);
	pathCoordinates(1, :) = origin;
	pathCoordinates(2, :) = insertion;
   
   return
else
   wrapOK = 1
end

% OK, that's the x and y bits sorted. Now work out the z coordinates

% The key point about the unwrapped problem is that the gradient is constant. 
% This means that the path around the cylinder is a helix that is continuous 
% with the line segments at each end.

% originZ: z coordinate of origin
% insertionZ: z coordinate of origin
% delz: height change
% delz:horizontal distance
% tangentPointZ1: origin tangent point
% tangetPointZ2: insertion tangent point

delz=insertionZ-originZ;

delx=l1+c+l2;

tangentPointZ1=originZ+(l1/delx) * delz;

tangentPointZ2=tangentPointZ1+(c/delx) * delz;

% now calculate some forces

originForceX = tangentPointX1 - originX;
originForceY = tangentPointY1 - originY;
originForceZ = tangentPointZ1 - originZ;
l = sqrt(originForceX^2 + originForceY^2 + originForceZ^2);
originForce(1) = originForceX * tension / l;
originForce(2) = originForceY * tension / l;
originForce(3) = originForceZ * tension / l;

insertionForceX = tangentPointX2 - insertionX;
insertionForceY = tangentPointY2 - insertionY;
insertionForceZ = tangentPointZ2 - insertionZ;
l = sqrt(insertionForceX^2 + insertionForceY^2 + insertionForceZ^2);
insertionForce(1) = insertionForceX * tension / l;
insertionForce(2) = insertionForceY * tension / l;
insertionForce(3) = insertionForceZ * tension / l;

cylinderForce(1) = -originForce(1) - insertionForce(1);
cylinderForce(2) = -originForce(2) - insertionForce(2);
cylinderForce(3) = -originForce(3) - insertionForce(3);

cylinderForcePosition(1) = 0;
cylinderForcePosition(2) = 0;
cylinderForcePosition(3) = (tangentPointZ1 + tangentPointZ2) / 2;

pathLength = sqrt(delx^2 + delz^2);

% that's the main calculations done. 
% Now check whether we need more points for drawing the path.

pathCoordinates = zeros(nExtraPathCoordinates + 4, 3);
pathCoordinates(1, :) = origin;
pathCoordinates(2, 1) = tangentPointX1 ;
pathCoordinates(2, 2) = tangentPointY1 ;
pathCoordinates(2, 3) = tangentPointZ1 ;
pathCoordinates(end, :) = insertion;
pathCoordinates(end - 1, 1) = tangentPointX2 ;
pathCoordinates(end - 1, 2) = tangentPointY2 ;
pathCoordinates(end - 1, 3) = tangentPointZ2 ;

delAngle = rho / (nExtraPathCoordinates + 1);
angle = tangentPointTheta1;
delZ = (tangentPointZ2 - tangentPointZ1) / (nExtraPathCoordinates + 1);
z = tangentPointZ1;
for i = 1: nExtraPathCoordinates
   angle = angle + delAngle;
   pathCoordinates(i + 2, 1) = r * cos(angle);
   pathCoordinates(i + 2, 2) = r * sin(angle);
   z = z + delZ;
   pathCoordinates(i + 2, 3) = z;
end

return

