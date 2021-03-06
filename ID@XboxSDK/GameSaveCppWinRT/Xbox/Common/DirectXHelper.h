//--------------------------------------------------------------------------------------
// DirectXHelper.h
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) : result(hr) {}

        virtual const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", result);
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }

	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}

    inline float GetScaleFactorForWindow(const RECT& viewportBounds)
    {
        static const float referenceWidth = 1920.f;
        static const float referenceHeight = 1080.f;
        static const float referenceAspectRatio = referenceWidth / referenceHeight;
        float viewportWidth = float(viewportBounds.right);
        float viewportHeight = float(viewportBounds.bottom);
        float aspectRatio = viewportWidth / viewportHeight;
        float scale = 1.f;

        if (aspectRatio <= referenceAspectRatio)
        {
            scale = viewportWidth / referenceWidth;
        }
        else // aspectRatio > referenceAspectRatio
        {
            scale = viewportHeight / referenceHeight;
        }

        return scale;
    }

    inline DirectX::XMMATRIX GetScaleMatrixForWindow(const RECT& viewportBounds)
    {
        float scale = GetScaleFactorForWindow(viewportBounds);
        return DirectX::XMMatrixScaling(scale, scale, 1.f);
    }

    inline DirectX::XMINT2 ConvertWindowPixelToLocalCoord(const RECT& viewportBounds, const DirectX::XMINT2& pixel)
    {
        float scale = 1.f / GetScaleFactorForWindow(viewportBounds);
        return DirectX::XMINT2(lround(float(pixel.x) * scale), lround(float(pixel.y) * scale));
    }

    inline bool IsPointInsideRectangle(const DirectX::XMINT2& point, const RECT& rectangle)
    {
        return (point.x >= rectangle.left
             && point.x <= rectangle.right
             && point.y >= rectangle.top
             && point.y <= rectangle.bottom);
    }

    inline bool IsPointInsideCircle(const DirectX::XMFLOAT2& point, const DirectX::XMFLOAT2& center, float radius)
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat2(&point);
        XMVECTOR x2 = XMLoadFloat2(&center);
        XMVECTOR V = XMVectorSubtract(x2, x1);
        XMVECTOR X = XMVector2LengthSq(V);
        float distanceSq = XMVectorGetX(X);
        float radiusSq = radius * radius;
        return distanceSq <= radiusSq;
    }
}
