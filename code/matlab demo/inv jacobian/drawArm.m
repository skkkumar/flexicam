function [ joints ] = drawArm( lengths, angles )
%drawArm Draw arm with the lengths and angles
%   Detailed explanation goes here


%plot the base
plot([0], [0], '*', 'color', 'g');


point = [0, 0];
angle = 0;
joints = zeros(3, 2);

%plot the robot arm
for i = 1 : 3
    newpoint = [0 0];
    newpoint(1) = point(1) + lengths(i) * cos(angle + angles(i));
    newpoint(2) = point(2) + lengths(i) * sin(angle + angles(i));
    
    plot([point(1) newpoint(1)], [point(2)  newpoint(2)], 'color', 'b');
    plot([newpoint(1)], [newpoint(2)], '*', 'color', 'b');
    
    point = newpoint;
    angle =  angles(i);
    joints(i, :) = point;
end

limit = 10;


limit = ceil( sum( abs(lengths) ));
axis([-limit, limit, -limit , limit]);

end

