//--------------------------------------------------------------------------------
//	表示文字保存するところ
//　labels.h
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  列挙型定義
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
	u8"言語選択"
};

static const char* kLanguage[eLanguageMax] =
{
	"English",
	u8"日本語"
};

static const char* kBackgroundColor[eLanguageMax] =
{
	"Background color",
	u8"背景色"
};

static const char* kCloseModelWindow[eLanguageMax] =
{
	"Close model window",
	u8"モデルウィンドウを閉じる"
};

static const char* kOpenModelWindow[eLanguageMax] =
{
	"Open model window",
	u8"モデルウィンドウを開く"
};

static const char* kCloseAnimatorWindow[eLanguageMax] =
{
	"Close animator window",
	u8"アニメーターウィンドウを閉じる"
};

static const char* kOpenAnimatorWindow[eLanguageMax] =
{
	"Open animator window",
	u8"アニメーターウィンドウを開く"
};

static const char* kCloseMaterialWindow[eLanguageMax] =
{
	"Close material window",
	u8"マテリアルウィンドウを閉じる"
};

static const char* kOpenMaterialWindow[eLanguageMax] =
{
	"Open material window",
	u8"マテリアルウィンドウを開く"
};

static const char* kCloseCameraWindow[eLanguageMax] =
{
	"Close camera window",
	u8"カメラウィンドウを閉じる"
};

static const char* kOpenCameraWindow[eLanguageMax] =
{
	"Open camera window",
	u8"カメラウィンドウを開く"
};

static const char* kUndisplaySkeleton[eLanguageMax] =
{
	"Undisplay skeletons",
	u8"骨の表示オフ"
};

static const char* kDisplaySkeleton[eLanguageMax] =
{
	"Display skeletons",
	u8"骨の表示オン"
};

static const char* kUndisplayMesh[eLanguageMax] =
{
	"Undisplay meshes",
	u8"メッシュの表示オフ"
};

static const char* kDisplayMesh[eLanguageMax] =
{
	"Display meshes",
	u8"メッシュの表示オン"
};

static const char* kUndisplayCollider[eLanguageMax] =
{
	"Undisplay colliders",
	u8"コライダーの表示オフ"
};

static const char* kDisplayCollider[eLanguageMax] =
{
	"Display colliders",
	u8"コライダーの表示オン"
};

static const char* kUndisplayBoundingSphere[eLanguageMax] =
{
	"Undisplay bounding spheres",
	u8"包囲球の表示オフ"
};

static const char* kDisplayBoundingSphere[eLanguageMax] =
{
	"Display bounding spheres",
	u8"包囲球の表示オン"
};

static const char* kFile[eLanguageMax] =
{
	"File",
	u8"ファイル"
};

static const char* kOpenFbxFile[eLanguageMax] =
{
	"Open fbx file",
	u8"Fbxファイルを開く"
};

static const char* kSaveAsJson[eLanguageMax] =
{
	"Save as Json",
	u8"Jsonとして保存"
};

static const char* kSaveAsBinary[eLanguageMax] =
{
	"Save as Binary",
	u8"Binaryとして保存"
};

static const char* kName[eLanguageMax] =
{
	"Name",
	u8"名前"
};

static const char* kRotationSpeed[eLanguageMax] =
{
	"Rotation speed",
	u8"回転速度"
};

static const char* kReverseTexcoordV[eLanguageMax] =
{
	"Reverse texcoord V",
	u8"UVのVを反転する"
};

static const char* kNodeInfo[eLanguageMax] =
{
	"Node info",
	u8"ノード情報"
};

static const char* kDelete[eLanguageMax] =
{
	"Delete",
	u8"削除"
};

static const char* kType[eLanguageMax] =
{
	"Type%d : %s",
	u8"タイプ%d : %s"
};

static const char* kTranslation[eLanguageMax] =
{
	"Translation",
	u8"位置"
};

static const char* kRotation[eLanguageMax] =
{
	"Rotation",
	u8"回転"
};

static const char* kQuaternion[eLanguageMax] =
{
	"Quaternion",
	u8"回転(クオタニオ)"
};

static const char* kScale[eLanguageMax] =
{
	"Scale",
	u8"スケール"
};

static const char* kMesh[eLanguageMax] =
{
	"Mesh",
	u8"メッシュ"
};

static const char* kBoundingSphereInfo[eLanguageMax] =
{
	"BoundingSphere :\nPosition : %.3f, %.3f, %.3f\nRadius : %.6f",
	u8"包囲球 :\n位置 : %.3f, %.3f, %.3f\n半径 : %.6f"
};

static const char* kDisattachBone[eLanguageMax] =
{
	"Disattach bone",
	u8"骨との連結を消す"
};

static const char* kPolygonNumber[eLanguageMax] =
{
	"Polygon number : %d",
	u8"ポリゴン数 : %d"
};

static const char* kVertexNumber[eLanguageMax] =
{
	"Vertex number : %d",
	u8"頂点数 : %d"
};

static const char* kIndexNumber[eLanguageMax] =
{
	"Index number : %d",
	u8"インデックス数 : %d"
};

static const char* kMaterialName[eLanguageMax] =
{
	"Material name",
	u8"マテリアル名"
};

static const char* kRenderPriority[eLanguageMax] =
{
	"Render priority",
	u8"描画順番"
};

static const char* kEnableLight[eLanguageMax] =
{
	"Light(for editor)",
	u8"ライトの適用(エディタ用)"
};

static const char* kEnableCullFace[eLanguageMax] =
{
	"Culling face(for editor)",
	u8"カリングの適用(エディタ用)"
};

static const char* kCastShadow[eLanguageMax] =
{
	"Cast shadow",
	u8"影の適用"
};

static const char* kShaderType[eLanguageMax] =
{
	"Shader type",
	u8"シェーダタイプ"
};

static const char* kColliderNumber[eLanguageMax] =
{
	"Collider number : %d",
	u8"コライダー数 : %d"
};

static const char* kEditNode[eLanguageMax] =
{
	"Edit",
	u8"編集"
};

static const char* kChildren[eLanguageMax] =
{
	"Children",
	u8"子供"
};

static const char* kCollider[eLanguageMax] =
{
	"Collider",
	u8"コライダー"
};

static const char* kTrigger[eLanguageMax] =
{
	"Trigger",
	u8"トリガー"
};

static const char* kAdd[eLanguageMax] =
{
	"Add",
	u8"追加"
};

static const char* kTransformVerteces[eLanguageMax] =
{
	"Transform verteces",
	u8"頂点変換"
};

static const char* kRecalculate[eLanguageMax] =
{
	"Recalculate",
	u8"再計算"
};

static const char* kIKController[eLanguageMax] =
{
	"IK controller",
	u8"IKコントローラ"
};

static const char* kMotion[eLanguageMax] =
{
	"Motion",
	u8"モーション"
};

static const char* kPause[eLanguageMax] =
{
	"Pause",
	u8"一時停止"
};

static const char* kPlay[eLanguageMax] =
{
	"Play",
	u8"プレイ"
};

static const char* kFrame[eLanguageMax] =
{
	"Frame",
	u8"フレーム"
};

static const char* kSelectMotion[eLanguageMax] =
{
	"Select motion",
	u8"モーション選択"
};

static const char* kMaterials[eLanguageMax] =
{
	"Materials",
	u8"マテリアル"
};

static const char* kDiffuse[eLanguageMax] =
{
	"Diffuse",
	u8"デフュース"
};

static const char* kAmbient[eLanguageMax] =
{
	"Ambient",
	u8"アンビエント"
};

static const char* kSpecular[eLanguageMax] =
{
	"Specular",
	u8"スペキュラ"
};

static const char* kEmissive[eLanguageMax] =
{
	"Emissive",
	u8"エミッシブ"
};

static const char* kPower[eLanguageMax] =
{
	"Power",
	u8"ハイライトパワー"
};

static const char* kColorTextureName[eLanguageMax] =
{
	"Color texture : %s",
	u8"色テクスチャ : %s"
};

static const char* kChangeColorTexture[eLanguageMax] =
{
	"Change color texture",
	u8"色テクスチャの変更"
};

static const char* kDiffuseTextureName[eLanguageMax] =
{
	"Diffuse texture : %s",
	u8"デフューステクスチャ : %s"
};

static const char* kChangeDiffuseTexture[eLanguageMax] =
{
	"Change diffuse texture",
	u8"デフューステクスチャの変更"
};

static const char* kDiffuseMaskName[eLanguageMax] =
{
	"Diffuse mask : %s",
	u8"デフュースマスク : %s"
};

static const char* kChangeDiffuseMask[eLanguageMax] =
{
	"Change diffuse mask",
	u8"デフュースマスクの変更"
};

static const char* kSpecularTextureName[eLanguageMax] =
{
	"Specular texture : %s",
	u8"スペキュラテクスチャ : %s"
};

static const char* kChangeSpecularTexture[eLanguageMax] =
{
	"Change specular texture",
	u8"スペキュラテクスチャの変更"
};

static const char* kSpecularMaskName[eLanguageMax] =
{
	"Specular mask : %s",
	u8"スペキュラマスク : %s"
};

static const char* kChangeSpecularMask[eLanguageMax] =
{
	"Change specular mask",
	u8"スペキュラマスクの変更"
};

static const char* kNormalTextureName[eLanguageMax] =
{
	"Normal texture : %s",
	u8"法線マップ : %s"
};

static const char* kChangeNormalTexture[eLanguageMax] =
{
	"Change normal texture",
	u8"法線マップの変更"
};

static const char* kDetailTextureName[eLanguageMax] =
{
	"Detail texture : %s",
	u8"詳細テクスチャ : %s"
};

static const char* kChangeDetailTexture[eLanguageMax] =
{
	"Change detail texture",
	u8"詳細テクスチャの変更"
};

static const char* kDetailMaskName[eLanguageMax] =
{
	"Detail mask : %s",
	u8"詳細テクスチャマスク : %s"
};

static const char* kChangeDetailMask[eLanguageMax] =
{
	"Change detail mask",
	u8"詳細テクスチャマスクの変更"
};

static const char* kTintByBaseMaskName[eLanguageMax] =
{
	"Tint by base mask : %s",
	u8"色テクスチャマスク : %s"
};

static const char* kChangeTintByBaseMask[eLanguageMax] =
{
	"Change Tint by base mask",
	u8"色テクスチャマスクの変更"
};

static const char* kRimMaskName[eLanguageMax] =
{
	"Rim mask : %s",
	u8"リムライトマスク : %s"
};

static const char* kChangeRimMask[eLanguageMax] =
{
	"Change rim mask",
	u8"リムライトマスクの変更"
};

static const char* kTranslucency[eLanguageMax] =
{
	"Translucency : %s",
	u8"透明度テクスチャ : %s"
};

static const char* kChangeTranslucency[eLanguageMax] =
{
	"Change translucency",
	u8"透明度テクスチャの変更"
};

static const char* kMetalnessMask[eLanguageMax] =
{
	"Metalness mask : %s",
	u8"ノー金属マスク : %s"
};

static const char* kChangeMetalnessMask[eLanguageMax] =
{
	"Change metalness mask",
	u8"ノー金属マスクの変更"
};

static const char* kSelfIllumMask[eLanguageMax] =
{
	"self illum mask : %s",
	u8"自発光マスク : %s"
};

static const char* kChangeSelfIllumMask[eLanguageMax] =
{
	"Change metalness mask",
	u8"ノー金属マスクの変更"
};

static const char* kFresnelWarpColor[eLanguageMax] =
{
	"Fresnel warp color : %s",
	u8"色のフレネル : %s"
};

static const char* kChangeFresnelWarpColor[eLanguageMax] =
{
	"Change fresnel warp color",
	u8"色のフレネルの変更"
};

static const char* kFresnelWarpRim[eLanguageMax] =
{
	"Fresnel warp rim : %s",
	u8"リムのフレネル : %s"
};

static const char* kChangeFresnelWarpRim[eLanguageMax] =
{
	"Change fresnel warp rim",
	u8"リムのフレネルの変更"
};

static const char* kFresnelWarpSpecular[eLanguageMax] =
{
	"Fresnel warp specular : %s",
	u8"スペキュラのフレネル : %s"
};

static const char* kChangeFresnelWarpSpecular[eLanguageMax] =
{
	"Change fresnel warp specular",
	u8"スペキュラのフレネルの変更"
};

static const char* kNewMaterial[eLanguageMax] =
{
	"New material",
	u8"新しいマテリアル"
};

static const char* kPositionAt[eLanguageMax] =
{
	"Position at",
	u8"注視点"
};

static const char* kPositionEye[eLanguageMax] =
{
	"Position eye",
	u8"カメラ位置"
};

static const char* kCurrentAnimation[eLanguageMax] =
{
	"Current motion",
	u8"今のモーション"
};

static const char* kEditFrame[eLanguageMax] =
{
	"Edit frames",
	u8"フレームの編集"
};

static const char* kStartFrame[eLanguageMax] =
{
	"Start frame",
	u8"開始フレーム"
};

static const char* kEndFrame[eLanguageMax] =
{
	"End frame",
	u8"終了フレーム"
};

static const char* kDeleteOutOfRangeFrames[eLanguageMax] =
{
	"Delete out of range frames",
	u8"範囲外のフレームの削除"
};

static const char* kStateTransition[eLanguageMax] =
{
	"State transition",
	u8"状態遷移"
};

static const char* kIsLoop[eLanguageMax] =
{
	"Is loop",
	u8"ループ"
};

static const char* kTransitionWhenIsOver[eLanguageMax] =
{
	"Transition when is over",
	u8"モーション終了時自動遷移"
};

static const char* kTransitionNotice[eLanguageMax] =
{
	"Unavailable when the [Is loop] flag is true",
	u8"ループがtrueの時は無効"
};

static const char* kOverFrame[eLanguageMax] =
{
	"The frame we think motion is over",
	u8"モーション自動遷移開始フレーム"
};

static const char* kBlendFrameNumber[eLanguageMax] =
{
	"The blend frame number",
	u8"モーションブレンドのフレーム数"
};

static const char* kNextAnimationName[eLanguageMax] =
{
	"Next motion name",
	u8"次のモーション名"
};

static const char* kTransition[eLanguageMax] =
{
	"Transition",
	u8"遷移"
};

static const char* kCondition[eLanguageMax] =
{
	"Condition",
	u8"条件"
};

static const char* kParameterType[eLanguageMax] =
{
	"Parameter type",
	u8"パラメーターの型"
};

static const char* kParameterName[eLanguageMax] =
{
	"Parameter name",
	u8"パラメーター名"
};

static const char* kOperator[eLanguageMax] =
{
	"Operator",
	u8"オペレーター"
};

static const char* kValue[eLanguageMax] =
{
	"Value",
	u8"値"
};

static const char* kSaveTransition[eLanguageMax] =
{
	"Save transition",
	u8"状態遷移のセーブ"
};

static const char* kChangeParent[eLanguageMax] =
{
	"Change parent",
	u8"親を変える"
};

static const char* kDeleteAnimation[eLanguageMax] =
{
	"Delete motion",
	u8"モーションの削除"
};

static const char* kAddAnimation[eLanguageMax] =
{
	"Add motion",
	u8"モーションの追加"
};

static const char* kAddTransition[eLanguageMax] =
{
	"Add transition",
	u8"遷移の追加"
};

static const char* kDeleteTransition[eLanguageMax] =
{
	"Delete transition",
	u8"遷移の削除"
};

static const char* kAddCondition[eLanguageMax] =
{
	"Add condition",
	u8"条件の追加"
};

static const char* kDeleteCondition[eLanguageMax] =
{
	"Delete condition",
	u8"条件の削除"
};