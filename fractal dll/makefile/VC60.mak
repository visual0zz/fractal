#���ļ���
NAME=stock

#����·��
PATH=../../IDE\VC60\Bin

LIB_PATH=../../IDE\VC60\Lib
INCLUDE_PATH=../../IDE\VC60\Include
BIN_PATH=../../Bin

#�����ļ�
OBJS=$(NAME).obj FuShu.obj FuShu4.obj

#���ӱ�־
LINK_FLAG=/libpath:$(LIB_PATH) /libpath:$(BIN_PATH) kernel32.lib user32.lib \
gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib \
odbccp32.lib /nologo /dll /incremental:no /machine:I386 \
/out:"$(BIN_PATH)/$(NAME).dll" /implib:"$(BIN_PATH)/$(NAME).lib" 

#�����־
CL_FLAG=/X /I $(INCLUDE_PATH) /nologo /MT /W3 /GX /O2 /D "WIN32" \
/D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "FFF_EXPORTS" /Fo"$(BIN_PATH)/" /Fd"$(BIN_PATH)/" /c

#����
$(NAME).exe:$(OBJS)
	@Link $(LINK_FLAG) $(OBJS)

#����
$(OBJS):$(NAME).cpp
FuShu.obj:FuShu.cpp FuShu.h
FuShu4.obj:FuShu4.cpp FuShu4.h

#��������
.cpp.obj:
	Cl $(CL_FLAG) $<

#����ļ�
clean:
