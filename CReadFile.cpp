// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CReadFile.h"
#include "os.h"

namespace irr
{
namespace io
{


CReadFile::CReadFile(const io::path& fileName)
: 
#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_
fh(0),
#else
File(0), 
#endif
FileSize(0), Filename(fileName)
{
	#ifdef _DEBUG
	setDebugName("CReadFile");
	#endif

	openFile();
}


CReadFile::~CReadFile()
{
#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_
	if (fh != -1)
		close(fh);
#else
	if (File)
		fclose(File);
#endif
}


//! returns how much was read
s32 CReadFile::read(void* buffer, u32 sizeToRead)
{
	if (!isOpen())
		return 0;

#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_
	return (s32)(KernelProxy::KPInstance())->read(fh, buffer, sizeToRead);
#else
	return (s32)fread(buffer, 1, sizeToRead, File);
#endif
}


//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CReadFile::seek(long finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;

#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_
	return (lseek(fh, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) >= 0);
#else
	return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
#endif
}


//! returns size of file
long CReadFile::getSize() const
{
	return FileSize;
}


//! returns where in the file we are.
long CReadFile::getPos() const
{
#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_
	return lseek(fh, 0, SEEK_CUR);
#else
	return ftell(File);
#endif
}


//! opens the file
void CReadFile::openFile()
{
	if (Filename.size() == 0) // bugfix posted by rt
	{
#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_
		fh = 0;
#else
		File = 0;
#endif
		return;
	}

#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_
	fh = open(Filename.c_str(), O_RDONLY);
	if (fh != -1)
	{
		// get FileSize
		lseek(fh, 0, SEEK_END);
		FileSize = getPos();
		lseek(fh, 0, SEEK_SET);
	} else {
		os::Printer::log("Error Reading File", ELL_INFORMATION);
	}
#else
#if defined ( _IRR_WCHAR_FILESYSTEM )
	File = _wfopen(Filename.c_str(), L"rb");
#else
	File = fopen(Filename.c_str(), "rb");
#endif
	if (File)
	{
		// get FileSize

		fseek(File, 0, SEEK_END);
		FileSize = getPos();
		fseek(File, 0, SEEK_SET);
	}
#endif

}


//! returns name of file
const io::path& CReadFile::getFileName() const
{
	return Filename;
}



IReadFile* createReadFile(const io::path& fileName)
{
	CReadFile* file = new CReadFile(fileName);
	if (file->isOpen())
		return file;

	file->drop();
	return 0;
}


} // end namespace io
} // end namespace irr

