

lengths = [5, 4, 1];
angles = [60, 30, 20] * 3.14 / 180;
alpha = 0.5;

close all;
figure;
hold on;

joints = drawArm(lengths, angles);


%get the goal and plot it
[x, y] = ginput(1);
plot([x], [y], '*', 'color', 'r');


de = [[x, y] - joints(3, :) 0]';
error = sqrt(sum(de.*de));

while (error >= 0.5)
    
    %compute J
    J = [];
    for i = 1 : 3
        if i == 1
            diff = [joints(3, :) 0];
        else
            diff = [joints(3, :) - joints(i-1,:) 0];
        end
        init = [0, 0, 1];
        j = [cross(init, diff)];
        J = [J; j];
    end
    J = J';
    
    %find delta angle
    J1 = pinv(J);
    de = [[x, y] - joints(3, :) 0]';
    theta = J1 * de;
    
    %find updated angle
    angles = angles + alpha * theta';
    
    joints = drawArm(lengths, angles);
    
    %find new error
    error = sqrt(sum(de.*de));
    
end
