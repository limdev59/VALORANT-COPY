#include "pch.h"
#include "Sova.h"
#include "CCore.h"

Sova::Sova()
    : CObject() {
}
Sova::~Sova() {
    if (model) delete model;
}

void Sova::setModel(AnimModel* _model) {
    model = _model;
}
void Sova::Update() {
    if (model) {
        //model->Update(position, rotation, scale);
    }
    hitboxCenter = position;
    hitboxSize = scale;
}
void Sova::Render() {
    if (model) {
        model->Render();
    }
}
