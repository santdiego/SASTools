tiempo = 60;
ts=0.1;
figure;
H1=scatter3(0,0,0,'Marker','.','MarkerFaceColor',[0.7 0.7 1],'MarkerEdgeColor',[0.7 0.7 1],'MarkerFaceAlpha',0.5);
hold;
H2=patch([0 0 1],[0 1 0],[0 0 0],'FaceAlpha',0.05);
view(-90,90);
xlabel('x');
ylabel('y');
axis equal;
for i=1:tiempo/ts
    x=hokuyo;
    data=plotHokuyo(double(x));
    set(H1,'XData',data(1,:),'YData',data(2,:),'ZData',data(3,:));
    set(H2,'XData',data(1,:),'YData',data(2,:),'ZData',data(3,:));
    drawnow;
    pause(ts);
end
