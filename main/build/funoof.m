function [f,oof]=funoof()

global N dist bitlens imp a b c d maxcodedlength
syms ri% 1/2 root of the 'inverse of rate'
per=eval(2^(-1.1^(d*ri^6+c*ri^4+b*ri^2+a)));%ri^4 for the sake of using newton method

%X = sym('x',[1 8]);

oof=0;
constraint=0;
syms lamda
for i=1:N
    syms (['x',num2str(i)]);    %x1、x2、x3。。。。
    tmpvar=eval(['x',num2str(i)]);
    %X(r,c)=tmpvar;
    tmpper=subs(per,{'ri'},{tmpvar});
    oof=oof+(1-(1-tmpper)^(bitlens(i)/1000))*imp(i)*dist(i);
    constraint=constraint+bitlens(i)*tmpvar^2;%-bitlens(i)*3;%1/3 overall codingrate
end
constraint=constraint-maxcodedlength;
constraint=eval(constraint);
lglrfun=oof+lamda/1000000*constraint;

index=0;
for i=1:N
    index=index+1;
    f(index)=diff(lglrfun,['x',num2str(i)]);
end

f(index+1)=diff(lglrfun,'lamda');%final functions

end
