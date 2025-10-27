#pragma once

class CObject;
class CScene
{
protected:
	GLboolean					loaded;
	vector<CObject*>			arrObj[(GLuint)GROUP_TYPE::END];
	wstring						strName;
public:
	CScene();
	~CScene();

	virtual void				Update() = 0;
	virtual void				Render() = 0;
	virtual void				Enter() = 0;
	virtual void				Exit() = 0;

	void setName(const wstring& _strName) {
		strName = _strName;
	}
	const wstring& getName() { return strName; }
	CObject& getObject(GROUP_TYPE type, size_t index) {
		if ((GLuint)type >= (GLuint)GROUP_TYPE::END || index >= arrObj[(GLuint)type].size()) {
			exit(9);
		}
		return *arrObj[(GLuint)type][index];
	}
protected:

	
	void addObject(CObject* obj, GROUP_TYPE type) {
		arrObj[(GLuint)type].push_back(obj);
	}
};

