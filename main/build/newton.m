function [x, distortion]=newton(x0,eps,round)%newton([0.8 0.8 0.8],0.00001,20)

global N

con=0;
%其中x0为迭代初值eps为精度要求N为最大迭代步数con用来记录结果是否收敛
[f,oof]=funoof();
df=dfun();

index=0;
xNplus1=cell(1,N+1);
for i=1:N
    index=index+1;
    xNplus1{index}=['x',num2str(i)];
    %x_val{index}=x0(index);
    %f(index)=diff(lglrfun,['x',num2str(r),'x',num2str(c)]);
end
xNplus1{N+1}='lamda';

for i=1:round
    x_val=num2cell(x0);
    f_val=vpa(subs(f,xNplus1,x_val));
    df_val=vpa(subs(df,xNplus1,x_val));

    x=x0-f_val/df_val;
    for j=1:length(x0)
        il(i,j)=x(j);
    end
    if norm(x-x0)<eps
        con=1;
        break;
    end
    %eval(x)
    for k=1:length(x)-1
        if double(x(k)^2>9.9)
            x(k)=9.89^0.5;
        end
    end
    %output result below
    x0=x;
    fprintf('Solution @round %d=',i);
    for j=1:length(x0)
        fprintf('%10.6f, ',double(x0(j)));
    end
    %fprintf('final solution: %d\n',double(x0));
    x_val=num2cell(x0);
    fprintf('\n  ----- distortion= %d\n',double(vpa(subs(oof,xNplus1,x_val))));
end
x_val=num2cell(x0);
%fprintf('final solution: %d\n',double(x0));
%fprintf('final distortion: %d\n',double(vpa(subs(oof,xNplus1,x_val))));
distortion=double(vpa(subs(oof,xNplus1,x_val)));
fprintf('\n');


%以下是将迭代过程写入txt文档文件名为iteration.txt
fid=fopen('iteration.txt','w');
fprintf(fid,'iteration');
for j=1:length(x0)
    fprintf(fid,' x%d',j);
end
for j= 1:i
    fprintf(fid,'\n%6d ',j);
    for k=1:length(x0)
        fprintf(fid,' %10.6f',double(il(j,k)));
    end
end
if con==1
    fprintf(fid,'\n计算结果收敛！ ');
end
if con==0
    fprintf(fid,'\n迭代步数过多可能不收敛！ ');
end
fclose(fid);
end