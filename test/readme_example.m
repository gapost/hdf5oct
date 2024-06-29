clear
if isfile('test.h5'), unlink('test.h5'); end

pkg load hdf5oct 

%% Write a uint32 matrix 
data = uint32(reshape(1:10,2,5)) % create data in workspace
h5create('test.h5','D1',size(data),'datatype','uint32')
h5write('test.h5','D1',data)
h5read('test.h5','D1')
h5read('test.h5','D1',[1 3],[2 2],[1 2])

%% Write a single string
oneliner = "This is a single string";
h5create('test.h5','D2',1,'datatype','string') % scalar string dataset
h5write('test.h5','D2',oneliner)
h5read('test.h5','D2')

%% Write multiple utf8 strings
str = {"one", "δύο", "три", "neljä"};
h5create('test.h5','D3',size(str),'datatype','string')
h5write('test.h5','D3',str)
h5read('test.h5','D3')
