//--------------------------------------------------------------------------------
//	�\�������ۑ�����Ƃ���
//�@labels.h
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �񋓌^��`
//--------------------------------------------------------------------------------
enum Language
{
	eEnglish = 0,
	eJapanese = 1,
	eLanguageMax
};

//--------------------------------------------------------------------------------
//  Labels
//--------------------------------------------------------------------------------
static const char* kSelectLanguage[eLanguageMax] =
{
	"Select language",
	u8"����I��"
};

static const char* kLanguage[eLanguageMax] =
{
	"English",
	u8"���{��"
};

static const char* kBackgroundColor[eLanguageMax] =
{
	"Background color",
	u8"�w�i�F"
};

static const char* kCloseModelWindow[eLanguageMax] =
{
	"Close model window",
	u8"���f���E�B���h�E�����"
};

static const char* kOpenModelWindow[eLanguageMax] =
{
	"Open model window",
	u8"���f���E�B���h�E���J��"
};

static const char* kCloseAnimatorWindow[eLanguageMax] =
{
	"Close animator window",
	u8"�A�j���[�^�[�E�B���h�E�����"
};

static const char* kOpenAnimatorWindow[eLanguageMax] =
{
	"Open animator window",
	u8"�A�j���[�^�[�E�B���h�E���J��"
};

static const char* kCloseMaterialWindow[eLanguageMax] =
{
	"Close material window",
	u8"�}�e���A���E�B���h�E�����"
};

static const char* kOpenMaterialWindow[eLanguageMax] =
{
	"Open material window",
	u8"�}�e���A���E�B���h�E���J��"
};

static const char* kCloseCameraWindow[eLanguageMax] =
{
	"Close camera window",
	u8"�J�����E�B���h�E�����"
};

static const char* kOpenCameraWindow[eLanguageMax] =
{
	"Open camera window",
	u8"�J�����E�B���h�E���J��"
};

static const char* kUndisplaySkeleton[eLanguageMax] =
{
	"Undisplay skeletons",
	u8"���̕\���I�t"
};

static const char* kDisplaySkeleton[eLanguageMax] =
{
	"Display skeletons",
	u8"���̕\���I��"
};

static const char* kUndisplayMesh[eLanguageMax] =
{
	"Undisplay meshes",
	u8"���b�V���̕\���I�t"
};

static const char* kDisplayMesh[eLanguageMax] =
{
	"Display meshes",
	u8"���b�V���̕\���I��"
};

static const char* kUndisplayCollider[eLanguageMax] =
{
	"Undisplay colliders",
	u8"�R���C�_�[�̕\���I�t"
};

static const char* kDisplayCollider[eLanguageMax] =
{
	"Display colliders",
	u8"�R���C�_�[�̕\���I��"
};

static const char* kUndisplayBoundingSphere[eLanguageMax] =
{
	"Undisplay bounding spheres",
	u8"��͋��̕\���I�t"
};

static const char* kDisplayBoundingSphere[eLanguageMax] =
{
	"Display bounding spheres",
	u8"��͋��̕\���I��"
};

static const char* kFile[eLanguageMax] =
{
	"File",
	u8"�t�@�C��"
};

static const char* kOpenFbxFile[eLanguageMax] =
{
	"Open fbx file",
	u8"Fbx�t�@�C�����J��"
};

static const char* kSaveAsJson[eLanguageMax] =
{
	"Save as Json",
	u8"Json�Ƃ��ĕۑ�"
};

static const char* kSaveAsBinary[eLanguageMax] =
{
	"Save as Binary",
	u8"Binary�Ƃ��ĕۑ�"
};

static const char* kName[eLanguageMax] =
{
	"Name",
	u8"���O"
};

static const char* kRotationSpeed[eLanguageMax] =
{
	"Rotation speed",
	u8"��]���x"
};

static const char* kReverseTexcoordV[eLanguageMax] =
{
	"Reverse texcoord V",
	u8"UV��V�𔽓]����"
};

static const char* kNodeInfo[eLanguageMax] =
{
	"Node info",
	u8"�m�[�h���"
};

static const char* kDelete[eLanguageMax] =
{
	"Delete",
	u8"�폜"
};

static const char* kType[eLanguageMax] =
{
	"Type%d : %s",
	u8"�^�C�v%d : %s"
};

static const char* kTranslation[eLanguageMax] =
{
	"Translation",
	u8"�ʒu"
};

static const char* kRotation[eLanguageMax] =
{
	"Rotation",
	u8"��]"
};

static const char* kQuaternion[eLanguageMax] =
{
	"Quaternion",
	u8"��](�N�I�^�j�I)"
};

static const char* kScale[eLanguageMax] =
{
	"Scale",
	u8"�X�P�[��"
};

static const char* kMesh[eLanguageMax] =
{
	"Mesh",
	u8"���b�V��"
};

static const char* kBoundingSphereInfo[eLanguageMax] =
{
	"BoundingSphere :\nPosition : %.3f, %.3f, %.3f\nRadius : %.6f",
	u8"��͋� :\n�ʒu : %.3f, %.3f, %.3f\n���a : %.6f"
};

static const char* kDisattachBone[eLanguageMax] =
{
	"Disattach bone",
	u8"���Ƃ̘A��������"
};

static const char* kPolygonNumber[eLanguageMax] =
{
	"Polygon number : %d",
	u8"�|���S���� : %d"
};

static const char* kVertexNumber[eLanguageMax] =
{
	"Vertex number : %d",
	u8"���_�� : %d"
};

static const char* kIndexNumber[eLanguageMax] =
{
	"Index number : %d",
	u8"�C���f�b�N�X�� : %d"
};

static const char* kMaterialName[eLanguageMax] =
{
	"Material name",
	u8"�}�e���A����"
};

static const char* kRenderPriority[eLanguageMax] =
{
	"Render priority",
	u8"�`�揇��"
};

static const char* kEnableLight[eLanguageMax] =
{
	"Light(for editor)",
	u8"���C�g�̓K�p(�G�f�B�^�p)"
};

static const char* kEnableCullFace[eLanguageMax] =
{
	"Culling face(for editor)",
	u8"�J�����O�̓K�p(�G�f�B�^�p)"
};

static const char* kCastShadow[eLanguageMax] =
{
	"Cast shadow",
	u8"�e�̓K�p"
};

static const char* kShaderType[eLanguageMax] =
{
	"Shader type",
	u8"�V�F�[�_�^�C�v"
};

static const char* kColliderNumber[eLanguageMax] =
{
	"Collider number : %d",
	u8"�R���C�_�[�� : %d"
};

static const char* kEditNode[eLanguageMax] =
{
	"Edit",
	u8"�ҏW"
};

static const char* kChildren[eLanguageMax] =
{
	"Children",
	u8"�q��"
};

static const char* kCollider[eLanguageMax] =
{
	"Collider",
	u8"�R���C�_�["
};

static const char* kTrigger[eLanguageMax] =
{
	"Trigger",
	u8"�g���K�["
};

static const char* kAdd[eLanguageMax] =
{
	"Add",
	u8"�ǉ�"
};

static const char* kTransformVerteces[eLanguageMax] =
{
	"Transform verteces",
	u8"���_�ϊ�"
};

static const char* kRecalculate[eLanguageMax] =
{
	"Recalculate",
	u8"�Čv�Z"
};

static const char* kIKController[eLanguageMax] =
{
	"IK controller",
	u8"IK�R���g���[��"
};

static const char* kMotion[eLanguageMax] =
{
	"Motion",
	u8"���[�V����"
};

static const char* kPause[eLanguageMax] =
{
	"Pause",
	u8"�ꎞ��~"
};

static const char* kPlay[eLanguageMax] =
{
	"Play",
	u8"�v���C"
};

static const char* kFrame[eLanguageMax] =
{
	"Frame",
	u8"�t���[��"
};

static const char* kSelectMotion[eLanguageMax] =
{
	"Select motion",
	u8"���[�V�����I��"
};

static const char* kMaterials[eLanguageMax] =
{
	"Materials",
	u8"�}�e���A��"
};

static const char* kDiffuse[eLanguageMax] =
{
	"Diffuse",
	u8"�f�t���[�X"
};

static const char* kAmbient[eLanguageMax] =
{
	"Ambient",
	u8"�A���r�G���g"
};

static const char* kSpecular[eLanguageMax] =
{
	"Specular",
	u8"�X�y�L����"
};

static const char* kEmissive[eLanguageMax] =
{
	"Emissive",
	u8"�G�~�b�V�u"
};

static const char* kPower[eLanguageMax] =
{
	"Power",
	u8"�n�C���C�g�p���["
};

static const char* kColorTextureName[eLanguageMax] =
{
	"Color texture : %s",
	u8"�F�e�N�X�`�� : %s"
};

static const char* kChangeColorTexture[eLanguageMax] =
{
	"Change color texture",
	u8"�F�e�N�X�`���̕ύX"
};

static const char* kDiffuseTextureName[eLanguageMax] =
{
	"Diffuse texture : %s",
	u8"�f�t���[�X�e�N�X�`�� : %s"
};

static const char* kChangeDiffuseTexture[eLanguageMax] =
{
	"Change diffuse texture",
	u8"�f�t���[�X�e�N�X�`���̕ύX"
};

static const char* kDiffuseMaskName[eLanguageMax] =
{
	"Diffuse mask : %s",
	u8"�f�t���[�X�}�X�N : %s"
};

static const char* kChangeDiffuseMask[eLanguageMax] =
{
	"Change diffuse mask",
	u8"�f�t���[�X�}�X�N�̕ύX"
};

static const char* kSpecularTextureName[eLanguageMax] =
{
	"Specular texture : %s",
	u8"�X�y�L�����e�N�X�`�� : %s"
};

static const char* kChangeSpecularTexture[eLanguageMax] =
{
	"Change specular texture",
	u8"�X�y�L�����e�N�X�`���̕ύX"
};

static const char* kSpecularMaskName[eLanguageMax] =
{
	"Specular mask : %s",
	u8"�X�y�L�����}�X�N : %s"
};

static const char* kChangeSpecularMask[eLanguageMax] =
{
	"Change specular mask",
	u8"�X�y�L�����}�X�N�̕ύX"
};

static const char* kNormalTextureName[eLanguageMax] =
{
	"Normal texture : %s",
	u8"�@���}�b�v : %s"
};

static const char* kChangeNormalTexture[eLanguageMax] =
{
	"Change normal texture",
	u8"�@���}�b�v�̕ύX"
};

static const char* kDetailTextureName[eLanguageMax] =
{
	"Detail texture : %s",
	u8"�ڍ׃e�N�X�`�� : %s"
};

static const char* kChangeDetailTexture[eLanguageMax] =
{
	"Change detail texture",
	u8"�ڍ׃e�N�X�`���̕ύX"
};

static const char* kDetailMaskName[eLanguageMax] =
{
	"Detail mask : %s",
	u8"�ڍ׃e�N�X�`���}�X�N : %s"
};

static const char* kChangeDetailMask[eLanguageMax] =
{
	"Change detail mask",
	u8"�ڍ׃e�N�X�`���}�X�N�̕ύX"
};

static const char* kTintByBaseMaskName[eLanguageMax] =
{
	"Tint by base mask : %s",
	u8"�F�e�N�X�`���}�X�N : %s"
};

static const char* kChangeTintByBaseMask[eLanguageMax] =
{
	"Change Tint by base mask",
	u8"�F�e�N�X�`���}�X�N�̕ύX"
};

static const char* kRimMaskName[eLanguageMax] =
{
	"Rim mask : %s",
	u8"�������C�g�}�X�N : %s"
};

static const char* kChangeRimMask[eLanguageMax] =
{
	"Change rim mask",
	u8"�������C�g�}�X�N�̕ύX"
};

static const char* kTranslucency[eLanguageMax] =
{
	"Translucency : %s",
	u8"�����x�e�N�X�`�� : %s"
};

static const char* kChangeTranslucency[eLanguageMax] =
{
	"Change translucency",
	u8"�����x�e�N�X�`���̕ύX"
};

static const char* kMetalnessMask[eLanguageMax] =
{
	"Metalness mask : %s",
	u8"�m�[�����}�X�N : %s"
};

static const char* kChangeMetalnessMask[eLanguageMax] =
{
	"Change metalness mask",
	u8"�m�[�����}�X�N�̕ύX"
};

static const char* kSelfIllumMask[eLanguageMax] =
{
	"self illum mask : %s",
	u8"�������}�X�N : %s"
};

static const char* kChangeSelfIllumMask[eLanguageMax] =
{
	"Change metalness mask",
	u8"�m�[�����}�X�N�̕ύX"
};

static const char* kFresnelWarpColor[eLanguageMax] =
{
	"Fresnel warp color : %s",
	u8"�F�̃t���l�� : %s"
};

static const char* kChangeFresnelWarpColor[eLanguageMax] =
{
	"Change fresnel warp color",
	u8"�F�̃t���l���̕ύX"
};

static const char* kFresnelWarpRim[eLanguageMax] =
{
	"Fresnel warp rim : %s",
	u8"�����̃t���l�� : %s"
};

static const char* kChangeFresnelWarpRim[eLanguageMax] =
{
	"Change fresnel warp rim",
	u8"�����̃t���l���̕ύX"
};

static const char* kFresnelWarpSpecular[eLanguageMax] =
{
	"Fresnel warp specular : %s",
	u8"�X�y�L�����̃t���l�� : %s"
};

static const char* kChangeFresnelWarpSpecular[eLanguageMax] =
{
	"Change fresnel warp specular",
	u8"�X�y�L�����̃t���l���̕ύX"
};

static const char* kNewMaterial[eLanguageMax] =
{
	"New material",
	u8"�V�����}�e���A��"
};

static const char* kPositionAt[eLanguageMax] =
{
	"Position at",
	u8"�����_"
};

static const char* kPositionEye[eLanguageMax] =
{
	"Position eye",
	u8"�J�����ʒu"
};

static const char* kCurrentAnimation[eLanguageMax] =
{
	"Current motion",
	u8"���̃��[�V����"
};

static const char* kEditFrame[eLanguageMax] =
{
	"Edit frames",
	u8"�t���[���̕ҏW"
};

static const char* kStartFrame[eLanguageMax] =
{
	"Start frame",
	u8"�J�n�t���[��"
};

static const char* kEndFrame[eLanguageMax] =
{
	"End frame",
	u8"�I���t���[��"
};

static const char* kDeleteOutOfRangeFrames[eLanguageMax] =
{
	"Delete out of range frames",
	u8"�͈͊O�̃t���[���̍폜"
};

static const char* kStateTransition[eLanguageMax] =
{
	"State transition",
	u8"��ԑJ��"
};

static const char* kIsLoop[eLanguageMax] =
{
	"Is loop",
	u8"���[�v"
};

static const char* kTransitionWhenIsOver[eLanguageMax] =
{
	"Transition when is over",
	u8"���[�V�����I���������J��"
};

static const char* kTransitionNotice[eLanguageMax] =
{
	"Unavailable when the [Is loop] flag is true",
	u8"���[�v��true�̎��͖���"
};

static const char* kOverFrame[eLanguageMax] =
{
	"The frame we think motion is over",
	u8"���[�V���������J�ڊJ�n�t���[��"
};

static const char* kBlendFrameNumber[eLanguageMax] =
{
	"The blend frame number",
	u8"���[�V�����u�����h�̃t���[����"
};

static const char* kNextAnimationName[eLanguageMax] =
{
	"Next motion name",
	u8"���̃��[�V������"
};

static const char* kTransition[eLanguageMax] =
{
	"Transition",
	u8"�J��"
};

static const char* kCondition[eLanguageMax] =
{
	"Condition",
	u8"����"
};

static const char* kParameterType[eLanguageMax] =
{
	"Parameter type",
	u8"�p�����[�^�[�̌^"
};

static const char* kParameterName[eLanguageMax] =
{
	"Parameter name",
	u8"�p�����[�^�[��"
};

static const char* kOperator[eLanguageMax] =
{
	"Operator",
	u8"�I�y���[�^�["
};

static const char* kValue[eLanguageMax] =
{
	"Value",
	u8"�l"
};

static const char* kSaveTransition[eLanguageMax] =
{
	"Save transition",
	u8"��ԑJ�ڂ̃Z�[�u"
};

static const char* kChangeParent[eLanguageMax] =
{
	"Change parent",
	u8"�e��ς���"
};

static const char* kDeleteAnimation[eLanguageMax] =
{
	"Delete motion",
	u8"���[�V�����̍폜"
};

static const char* kAddAnimation[eLanguageMax] =
{
	"Add motion",
	u8"���[�V�����̒ǉ�"
};

static const char* kAddTransition[eLanguageMax] =
{
	"Add transition",
	u8"�J�ڂ̒ǉ�"
};

static const char* kDeleteTransition[eLanguageMax] =
{
	"Delete transition",
	u8"�J�ڂ̍폜"
};

static const char* kAddCondition[eLanguageMax] =
{
	"Add condition",
	u8"�����̒ǉ�"
};

static const char* kDeleteCondition[eLanguageMax] =
{
	"Delete condition",
	u8"�����̍폜"
};