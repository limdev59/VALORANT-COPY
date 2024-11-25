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

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Enter() = 0;
	virtual void Exit() = 0;

	void SetName(const wstring& _strName) {
		strName = _strName;
	}
	const wstring& GetName() { return strName; }

protected:
	void AddObject(CObject* obj, GROUP_TYPE type) {
		arrObj[(GLuint)type].push_back(obj);
	}
};

