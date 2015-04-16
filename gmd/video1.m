%X,Y,Z contains the features average target velocity, directional distance
%and target trajectory mean respectively of the normal crowd behaviour and
%A,B,C contains the features of the abnormal crowd behaviour.
%v1 contains the pdf values.

X=load ('avg_vel.txt')
Y=load ('directional_dist.txt')
Z=load ('trajectory_mean.txt')
X=[X(1:2:end)' X(2:2:end)'];
Y=[Y(1:2:end)' Y(2:2:end)'];
Z=[Z(1:2:end)' Z(2:2:end)'];
obj=gmdistribution.fit([X,Y,Z],3);
A=load ('avg_vel_an.txt')
B=load ('directional_dist_an.txt')
C=load ('trajectory_mean_an.txt')
A=[A(1:2:end)' A(2:2:end)'];
B=[B(1:2:end)' B(2:2:end)'];
C=[C(1:2:end)' C(2:2:end)'];
v1=pdf(obj,[A,B,C]);
