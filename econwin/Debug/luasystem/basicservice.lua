--����������

--�����µĶ�ʱ������
function timer(timems)
	local t = {};

	--newtimer
	t.id = newtimer();
	t.PT = timems;--ms
	t.Q =false;--����ź�
	t.INPUT =false;--�����ź�
	t.oldIN =false;
	t.setms = function (self,ms)
		self.PT = ms ;
		end
	t.checkin = function (self,input)
		if(input == 0) then
			input = false;
			self.INPUT = input;
		elseif((input == 1)) then
			input = true;
			self.INPUT = input;
		else
			self.INPUT = input;
		end
		if( (not self.oldIN) and (self.INPUT)) then--����
			--����ʱ���
			starttimer(self.id);
		elseif((self.oldIN) and (self.INPUT)) then--һֱΪ��
			if(timerelapsed(self.id)> self.PT) then
				self.Q = true;
			end
		else
			self.Q = false;
		end

		self.oldIN = self.INPUT;
		end


	return t;
end

--�����µ�R_TRIG
function R_TRIG()
	local t = {};

	--newtimer
	t.Q =false;--����ź�
	t.INPUT =false;--�����ź�
	t.oldIN =false;

	t.checkin = function (self,input)
		if(input == 0) then
			input = false;
			self.INPUT = input;
		elseif((input == 1)) then
			input = true;
			self.INPUT = input;
		else
			self.INPUT = input;
		end
		if( (not self.oldIN ) and (self.INPUT)) then--����
			self.Q = true;
		else
			self.Q = false;
		end

		self.oldIN = self.INPUT;
	end


	return t;
end
--�����µ�F_TRIG
function F_TRIG()
	local t = {};

	--newtimer
	t.Q = false;--����ź�
	t.INPUT =false;--�����ź�
	t.oldIN =false;

	t.checkin = function (self,input)
		if(input == 0) then
			input = false;
			self.INPUT = input;
		elseif((input == 1)) then
			input = true;
			self.INPUT = input;
		else
			self.INPUT = input;
		end
		if( (self.oldIN) and (not self.INPUT)) then--�½���
			self.Q = true;
		else
			self.Q = false;
		end

		self.oldIN = self.INPUT;
	end


	return t;
end
--�û������ļ�ע��
--����:fileregister("usertask1.lua");
filenamelist = {};
filenamelistlen = 0;

function fileregister(filename)
	filenamelist[#filenamelist + 1] = filename;
	filenamelistlen = #filenamelist;
end
