lags = load('asynctest.log')';
iodd = find(mod(lags(1,:),2));
ieven = find(~mod(lags(1,:),2));

old = lags(2:end, iodd);
modern = lags(2:end, ieven);

figure;
subplot(2,1,1);
[n,x] = hist(modern(:),50);
bar(x,n./sum(n),.5,'hist');
ylabel('p(latency)')
set(gca,'FontSize',12);
h_ylabel = get(gca,'YLabel');
set(h_ylabel,'FontSize',12); 



subplot(2,1,2); 
[n,x] =hist(old(:),50);
bar(x,n./sum(n),.5,'hist');
ylabel('p(latency)')
xlabel('latency (ms)','FontSize',12);
set(gca,'FontSize',12);

h_ylabel = get(gca,'YLabel');
set(h_ylabel,'FontSize',12); 

