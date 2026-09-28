///////////////////////////////////////////////////////////////////////////////
//  D3DXMath64.cpp : x64 replacements for the D3DX math entry points
//
//  The DirectX SDK shipped with this tree (DX9bLink) only carries 32-bit
//  import/static libraries, so the x64 server configurations cannot link
//  d3dx8.lib / d3dx9.lib.
///////////////////////////////////////////////////////////////////////////////

#if defined(_M_X64) || defined(D3DXMATH64_TEST)

#include <windows.h>
#include <d3dx9math.h>

FLOAT WINAPI D3DXMatrixDeterminant(CONST D3DXMATRIX *pM)
{
	FLOAT s0 = pM->m[0][0] * pM->m[1][1] - pM->m[1][0] * pM->m[0][1];
	FLOAT s1 = pM->m[0][0] * pM->m[1][2] - pM->m[1][0] * pM->m[0][2];
	FLOAT s2 = pM->m[0][0] * pM->m[1][3] - pM->m[1][0] * pM->m[0][3];
	FLOAT s3 = pM->m[0][1] * pM->m[1][2] - pM->m[1][1] * pM->m[0][2];
	FLOAT s4 = pM->m[0][1] * pM->m[1][3] - pM->m[1][1] * pM->m[0][3];
	FLOAT s5 = pM->m[0][2] * pM->m[1][3] - pM->m[1][2] * pM->m[0][3];

	FLOAT c5 = pM->m[2][2] * pM->m[3][3] - pM->m[3][2] * pM->m[2][3];
	FLOAT c4 = pM->m[2][1] * pM->m[3][3] - pM->m[3][1] * pM->m[2][3];
	FLOAT c3 = pM->m[2][1] * pM->m[3][2] - pM->m[3][1] * pM->m[2][2];
	FLOAT c2 = pM->m[2][0] * pM->m[3][3] - pM->m[3][0] * pM->m[2][3];
	FLOAT c1 = pM->m[2][0] * pM->m[3][2] - pM->m[3][0] * pM->m[2][2];
	FLOAT c0 = pM->m[2][0] * pM->m[3][1] - pM->m[3][0] * pM->m[2][1];

	return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
}

D3DXMATRIX* WINAPI D3DXMatrixInverse(D3DXMATRIX *pOut, FLOAT *pDeterminant, CONST D3DXMATRIX *pM)
{
	CONST FLOAT (*m)[4] = pM->m;

	FLOAT s0 = m[0][0] * m[1][1] - m[1][0] * m[0][1];
	FLOAT s1 = m[0][0] * m[1][2] - m[1][0] * m[0][2];
	FLOAT s2 = m[0][0] * m[1][3] - m[1][0] * m[0][3];
	FLOAT s3 = m[0][1] * m[1][2] - m[1][1] * m[0][2];
	FLOAT s4 = m[0][1] * m[1][3] - m[1][1] * m[0][3];
	FLOAT s5 = m[0][2] * m[1][3] - m[1][2] * m[0][3];

	FLOAT c5 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	FLOAT c4 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	FLOAT c3 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	FLOAT c2 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	FLOAT c1 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	FLOAT c0 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

	FLOAT det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
	if (det == 0.0f)
	{
		return NULL;
	}
	if (pDeterminant)
	{
		*pDeterminant = det;
	}

	FLOAT invdet = 1.0f / det;
	D3DXMATRIX out;

	out.m[0][0] = ( m[1][1] * c5 - m[1][2] * c4 + m[1][3] * c3) * invdet;
	out.m[0][1] = (-m[0][1] * c5 + m[0][2] * c4 - m[0][3] * c3) * invdet;
	out.m[0][2] = ( m[3][1] * s5 - m[3][2] * s4 + m[3][3] * s3) * invdet;
	out.m[0][3] = (-m[2][1] * s5 + m[2][2] * s4 - m[2][3] * s3) * invdet;

	out.m[1][0] = (-m[1][0] * c5 + m[1][2] * c2 - m[1][3] * c1) * invdet;
	out.m[1][1] = ( m[0][0] * c5 - m[0][2] * c2 + m[0][3] * c1) * invdet;
	out.m[1][2] = (-m[3][0] * s5 + m[3][2] * s2 - m[3][3] * s1) * invdet;
	out.m[1][3] = ( m[2][0] * s5 - m[2][2] * s2 + m[2][3] * s1) * invdet;

	out.m[2][0] = ( m[1][0] * c4 - m[1][1] * c2 + m[1][3] * c0) * invdet;
	out.m[2][1] = (-m[0][0] * c4 + m[0][1] * c2 - m[0][3] * c0) * invdet;
	out.m[2][2] = ( m[3][0] * s4 - m[3][1] * s2 + m[3][3] * s0) * invdet;
	out.m[2][3] = (-m[2][0] * s4 + m[2][1] * s2 - m[2][3] * s0) * invdet;

	out.m[3][0] = (-m[1][0] * c3 + m[1][1] * c1 - m[1][2] * c0) * invdet;
	out.m[3][1] = ( m[0][0] * c3 - m[0][1] * c1 + m[0][2] * c0) * invdet;
	out.m[3][2] = (-m[3][0] * s3 + m[3][1] * s1 - m[3][2] * s0) * invdet;
	out.m[3][3] = ( m[2][0] * s3 - m[2][1] * s1 + m[2][2] * s0) * invdet;

	*pOut = out;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixMultiply(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2)
{
	D3DXMATRIX out;
	INT i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			out.m[i][j] = pM1->m[i][0] * pM2->m[0][j]
						+ pM1->m[i][1] * pM2->m[1][j]
						+ pM1->m[i][2] * pM2->m[2][j]
						+ pM1->m[i][3] * pM2->m[3][j];
		}
	}
	*pOut = out;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixMultiplyTranspose(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2)
{
	D3DXMATRIX tmp;
	D3DXMatrixMultiply(&tmp, pM1, pM2);
	return D3DXMatrixTranspose(pOut, &tmp);
}

D3DXMATRIX* WINAPI D3DXMatrixTranspose(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM)
{
	D3DXMATRIX out;
	INT i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			out.m[i][j] = pM->m[j][i];
		}
	}
	*pOut = out;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixScaling(D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[0][0] = sx;
	pOut->m[1][1] = sy;
	pOut->m[2][2] = sz;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixTranslation(D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[3][0] = x;
	pOut->m[3][1] = y;
	pOut->m[3][2] = z;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixRotationX(D3DXMATRIX *pOut, FLOAT angle)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[1][1] =  cosf(angle);
	pOut->m[2][2] =  cosf(angle);
	pOut->m[1][2] =  sinf(angle);
	pOut->m[2][1] = -sinf(angle);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixRotationY(D3DXMATRIX *pOut, FLOAT angle)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[0][0] =  cosf(angle);
	pOut->m[2][2] =  cosf(angle);
	pOut->m[0][2] = -sinf(angle);
	pOut->m[2][0] =  sinf(angle);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixRotationZ(D3DXMATRIX *pOut, FLOAT angle)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[0][0] =  cosf(angle);
	pOut->m[1][1] =  cosf(angle);
	pOut->m[0][1] =  sinf(angle);
	pOut->m[1][0] = -sinf(angle);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixRotationAxis(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pV, FLOAT angle)
{
	D3DXVECTOR3 v;
	D3DXVec3Normalize(&v, pV);

	FLOAT c = cosf(angle);
	FLOAT s = sinf(angle);
	FLOAT t = 1.0f - c;

	D3DXMatrixIdentity(pOut);
	pOut->m[0][0] = t * v.x * v.x + c;
	pOut->m[0][1] = t * v.x * v.y + s * v.z;
	pOut->m[0][2] = t * v.x * v.z - s * v.y;
	pOut->m[1][0] = t * v.y * v.x - s * v.z;
	pOut->m[1][1] = t * v.y * v.y + c;
	pOut->m[1][2] = t * v.y * v.z + s * v.x;
	pOut->m[2][0] = t * v.z * v.x + s * v.y;
	pOut->m[2][1] = t * v.z * v.y - s * v.x;
	pOut->m[2][2] = t * v.z * v.z + c;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixRotationQuaternion(D3DXMATRIX *pOut, CONST D3DXQUATERNION *pQ)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[0][0] = 1.0f - 2.0f * (pQ->y * pQ->y + pQ->z * pQ->z);
	pOut->m[0][1] =        2.0f * (pQ->x * pQ->y + pQ->z * pQ->w);
	pOut->m[0][2] =        2.0f * (pQ->x * pQ->z - pQ->y * pQ->w);
	pOut->m[1][0] =        2.0f * (pQ->x * pQ->y - pQ->z * pQ->w);
	pOut->m[1][1] = 1.0f - 2.0f * (pQ->x * pQ->x + pQ->z * pQ->z);
	pOut->m[1][2] =        2.0f * (pQ->y * pQ->z + pQ->x * pQ->w);
	pOut->m[2][0] =        2.0f * (pQ->x * pQ->z + pQ->y * pQ->w);
	pOut->m[2][1] =        2.0f * (pQ->y * pQ->z - pQ->x * pQ->w);
	pOut->m[2][2] = 1.0f - 2.0f * (pQ->x * pQ->x + pQ->y * pQ->y);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixLookAtLH(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt, CONST D3DXVECTOR3 *pUp)
{
	D3DXVECTOR3 right, rightn, up, upn, vec, vec2;

	vec2 = *pAt - *pEye;
	D3DXVec3Normalize(&vec, &vec2);
	D3DXVec3Cross(&right, pUp, &vec);
	D3DXVec3Cross(&up, &vec, &right);
	D3DXVec3Normalize(&rightn, &right);
	D3DXVec3Normalize(&upn, &up);

	pOut->m[0][0] = rightn.x;
	pOut->m[1][0] = rightn.y;
	pOut->m[2][0] = rightn.z;
	pOut->m[3][0] = -D3DXVec3Dot(&rightn, pEye);
	pOut->m[0][1] = upn.x;
	pOut->m[1][1] = upn.y;
	pOut->m[2][1] = upn.z;
	pOut->m[3][1] = -D3DXVec3Dot(&upn, pEye);
	pOut->m[0][2] = vec.x;
	pOut->m[1][2] = vec.y;
	pOut->m[2][2] = vec.z;
	pOut->m[3][2] = -D3DXVec3Dot(&vec, pEye);
	pOut->m[0][3] = 0.0f;
	pOut->m[1][3] = 0.0f;
	pOut->m[2][3] = 0.0f;
	pOut->m[3][3] = 1.0f;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixLookAtRH(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt, CONST D3DXVECTOR3 *pUp)
{
	D3DXVECTOR3 right, rightn, up, upn, vec, vec2;

	vec2 = *pAt - *pEye;
	D3DXVec3Normalize(&vec, &vec2);
	D3DXVec3Cross(&right, pUp, &vec);
	D3DXVec3Cross(&up, &vec, &right);
	D3DXVec3Normalize(&rightn, &right);
	D3DXVec3Normalize(&upn, &up);

	pOut->m[0][0] = -rightn.x;
	pOut->m[1][0] = -rightn.y;
	pOut->m[2][0] = -rightn.z;
	pOut->m[3][0] =  D3DXVec3Dot(&rightn, pEye);
	pOut->m[0][1] =  upn.x;
	pOut->m[1][1] =  upn.y;
	pOut->m[2][1] =  upn.z;
	pOut->m[3][1] = -D3DXVec3Dot(&upn, pEye);
	pOut->m[0][2] = -vec.x;
	pOut->m[1][2] = -vec.y;
	pOut->m[2][2] = -vec.z;
	pOut->m[3][2] =  D3DXVec3Dot(&vec, pEye);
	pOut->m[0][3] = 0.0f;
	pOut->m[1][3] = 0.0f;
	pOut->m[2][3] = 0.0f;
	pOut->m[3][3] = 1.0f;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixPerspectiveFovLH(D3DXMATRIX *pOut, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[0][0] = 1.0f / (aspect * tanf(fovy / 2.0f));
	pOut->m[1][1] = 1.0f / tanf(fovy / 2.0f);
	pOut->m[2][2] = zf / (zf - zn);
	pOut->m[2][3] = 1.0f;
	pOut->m[3][2] = (zf * zn) / (zn - zf);
	pOut->m[3][3] = 0.0f;
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixPerspectiveFovRH(D3DXMATRIX *pOut, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf)
{
	D3DXMatrixIdentity(pOut);
	pOut->m[0][0] = 1.0f / (aspect * tanf(fovy / 2.0f));
	pOut->m[1][1] = 1.0f / tanf(fovy / 2.0f);
	pOut->m[2][2] = zf / (zn - zf);
	pOut->m[2][3] = -1.0f;
	pOut->m[3][2] = (zf * zn) / (zn - zf);
	pOut->m[3][3] = 0.0f;
	return pOut;
}

D3DXVECTOR3* WINAPI D3DXVec3Normalize(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV)
{
	FLOAT norm = D3DXVec3Length(pV);
	if (norm == 0.0f)
	{
		pOut->x = 0.0f;
		pOut->y = 0.0f;
		pOut->z = 0.0f;
	}
	else
	{
		pOut->x = pV->x / norm;
		pOut->y = pV->y / norm;
		pOut->z = pV->z / norm;
	}
	return pOut;
}

D3DXVECTOR3* WINAPI D3DXVec3TransformCoord(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
{
	D3DXVECTOR3 v = *pV;
	FLOAT norm = pM->m[0][3] * v.x + pM->m[1][3] * v.y + pM->m[2][3] * v.z + pM->m[3][3];

	pOut->x = (pM->m[0][0] * v.x + pM->m[1][0] * v.y + pM->m[2][0] * v.z + pM->m[3][0]) / norm;
	pOut->y = (pM->m[0][1] * v.x + pM->m[1][1] * v.y + pM->m[2][1] * v.z + pM->m[3][1]) / norm;
	pOut->z = (pM->m[0][2] * v.x + pM->m[1][2] * v.y + pM->m[2][2] * v.z + pM->m[3][2]) / norm;
	return pOut;
}

D3DXVECTOR3* WINAPI D3DXVec3TransformNormal(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
{
	D3DXVECTOR3 v = *pV;

	pOut->x = pM->m[0][0] * v.x + pM->m[1][0] * v.y + pM->m[2][0] * v.z;
	pOut->y = pM->m[0][1] * v.x + pM->m[1][1] * v.y + pM->m[2][1] * v.z;
	pOut->z = pM->m[0][2] * v.x + pM->m[1][2] * v.y + pM->m[2][2] * v.z;
	return pOut;
}

D3DXVECTOR4* WINAPI D3DXVec3Transform(D3DXVECTOR4 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
{
	D3DXVECTOR3 v = *pV;

	pOut->x = pM->m[0][0] * v.x + pM->m[1][0] * v.y + pM->m[2][0] * v.z + pM->m[3][0];
	pOut->y = pM->m[0][1] * v.x + pM->m[1][1] * v.y + pM->m[2][1] * v.z + pM->m[3][1];
	pOut->z = pM->m[0][2] * v.x + pM->m[1][2] * v.y + pM->m[2][2] * v.z + pM->m[3][2];
	pOut->w = pM->m[0][3] * v.x + pM->m[1][3] * v.y + pM->m[2][3] * v.z + pM->m[3][3];
	return pOut;
}

D3DXQUATERNION* WINAPI D3DXQuaternionNormalize(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ)
{
	FLOAT norm = D3DXQuaternionLength(pQ);
	if (norm == 0.0f)
	{
		pOut->x = pOut->y = pOut->z = pOut->w = 0.0f;
	}
	else
	{
		pOut->x = pQ->x / norm;
		pOut->y = pQ->y / norm;
		pOut->z = pQ->z / norm;
		pOut->w = pQ->w / norm;
	}
	return pOut;
}

D3DXQUATERNION* WINAPI D3DXQuaternionMultiply(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1, CONST D3DXQUATERNION *pQ2)
{
	D3DXQUATERNION out;
	out.x = pQ2->w * pQ1->x + pQ2->x * pQ1->w + pQ2->y * pQ1->z - pQ2->z * pQ1->y;
	out.y = pQ2->w * pQ1->y - pQ2->x * pQ1->z + pQ2->y * pQ1->w + pQ2->z * pQ1->x;
	out.z = pQ2->w * pQ1->z + pQ2->x * pQ1->y - pQ2->y * pQ1->x + pQ2->z * pQ1->w;
	out.w = pQ2->w * pQ1->w - pQ2->x * pQ1->x - pQ2->y * pQ1->y - pQ2->z * pQ1->z;
	*pOut = out;
	return pOut;
}

D3DXQUATERNION* WINAPI D3DXQuaternionRotationAxis(D3DXQUATERNION *pOut, CONST D3DXVECTOR3 *pV, FLOAT angle)
{
	D3DXVECTOR3 v;
	D3DXVec3Normalize(&v, pV);

	pOut->x = sinf(angle / 2.0f) * v.x;
	pOut->y = sinf(angle / 2.0f) * v.y;
	pOut->z = sinf(angle / 2.0f) * v.z;
	pOut->w = cosf(angle / 2.0f);
	return pOut;
}

D3DXQUATERNION* WINAPI D3DXQuaternionSlerp(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1, CONST D3DXQUATERNION *pQ2, FLOAT t)
{
	FLOAT epsilon = 1.0f;
	FLOAT temp = 1.0f - t;
	FLOAT u = t;
	FLOAT dot = D3DXQuaternionDot(pQ1, pQ2);

	if (dot < 0.0f)
	{
		epsilon = -1.0f;
		dot = -dot;
	}
	if (1.0f - dot > 0.001f)
	{
		FLOAT theta = acosf(dot);
		temp = sinf(theta * temp) / sinf(theta);
		u    = sinf(theta * u)    / sinf(theta);
	}

	pOut->x = temp * pQ1->x + epsilon * u * pQ2->x;
	pOut->y = temp * pQ1->y + epsilon * u * pQ2->y;
	pOut->z = temp * pQ1->z + epsilon * u * pQ2->z;
	pOut->w = temp * pQ1->w + epsilon * u * pQ2->w;
	return pOut;
}

D3DXPLANE* WINAPI D3DXPlaneNormalize(D3DXPLANE *pOut, CONST D3DXPLANE *pP)
{
	FLOAT norm = sqrtf(pP->a * pP->a + pP->b * pP->b + pP->c * pP->c);
	if (norm == 0.0f)
	{
		pOut->a = pOut->b = pOut->c = pOut->d = 0.0f;
	}
	else
	{
		pOut->a = pP->a / norm;
		pOut->b = pP->b / norm;
		pOut->c = pP->c / norm;
		pOut->d = pP->d / norm;
	}
	return pOut;
}

D3DXPLANE* WINAPI D3DXPlaneFromPointNormal(D3DXPLANE *pOut, CONST D3DXVECTOR3 *pPoint, CONST D3DXVECTOR3 *pNormal)
{
	pOut->a = pNormal->x;
	pOut->b = pNormal->y;
	pOut->c = pNormal->z;
	pOut->d = -D3DXVec3Dot(pPoint, pNormal);
	return pOut;
}

D3DXPLANE* WINAPI D3DXPlaneFromPoints(D3DXPLANE *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2, CONST D3DXVECTOR3 *pV3)
{
	D3DXVECTOR3 edge1 = *pV2 - *pV1;
	D3DXVECTOR3 edge2 = *pV3 - *pV1;
	D3DXVECTOR3 normal, Nnormal;

	D3DXVec3Cross(&normal, &edge1, &edge2);
	D3DXVec3Normalize(&Nnormal, &normal);
	D3DXPlaneFromPointNormal(pOut, pV1, &Nnormal);
	return pOut;
}

#endif // _M_X64
