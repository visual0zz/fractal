#���ļ���
NAME=fractal

#����·��
PATH=../../IDE\VC60\Bin

LIB_PATH=../../IDE\VC60\Lib
INCLUDE_PATH=../../IDE\VC60\Include
BIN_PATH=../../Bin

#�����ļ�
OBJS=$(NAME).obj FuShu.obj FuShu4.obj

#��Դ�ļ�
RES=$(NAME).res

#���ӱ�־
LINK_FLAG=/libpath:$(LIB_PATH) /libpath:$(BIN_PATH) kernel32.lib user32.lib gdi32.lib winspool.lib \
comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib \
uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /machine:I386 \
/out:"$(BIN_PATH)/$(NAME).exe"

#�����־
CL_FLAG=/X /I $(INCLUDE_PATH) /nologo /ML /W3 /GX /O2 /D "WIN32" \
/D "NDEBUG" /D "_WINDOWS" /Fo"$(BIN_PATH)/" /c

#��Դ��־
RC_FLAG=/l 0x804 /fo"$(BIN_PATH)/$(NAME).res" /d "NDEBUG"

#����
$(NAME).exe:$(OBJS) $(RES)
	@Link $(LINK_FLAG) $(OBJS) $(RES)

#����
$(OBJS):$(NAME).cpp
FuShu.obj:FuShu.cpp FuShu.h
FuShu4.obj:FuShu4.cpp FuShu4.h

#��Դ����
$(RES):$(NAME).rc


#��������
.cpp.obj:
	@Cl $(CL_FLAG) $<

.rc.res:
	@Rc $(RC_FLAG) $<

#����ļ�
clean:
