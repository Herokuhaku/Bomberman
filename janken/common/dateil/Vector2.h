template<class T>
Vector2Temple<T>::Vector2Temple()
{
	x = 0;
	y = 0;
}
template<class T>
Vector2Temple<T>::Vector2Temple(T x, T y)
{
	//this->x = x;
	//this->y = y;
	Vector2Temple::x = x;
	Vector2Temple::y = y;
}
template<class T>
Vector2Temple<T>::~Vector2Temple()
{
}
template<class T>
Vector2Temple<T>& Vector2Temple<T>::operator=(const Vector2Temple<T>& vec)
{
	x = vec.x;
	y = vec.y;

	return *this;
}
template<class T>
T& Vector2Temple<T>::operator[](int no)
{
	if (no == 0)
	{
		return x;
	}
	else if (no == 1)
	{
		return y;
	}
	else
	{
		return x;
	}
}
template<class T>
Vector2Temple<T>& Vector2Temple<T>::operator+=(const Vector2Temple<T>& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}
template<class T>
Vector2Temple<T>& Vector2Temple<T>::operator-=(const Vector2Temple<T>& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}
template<class T>
Vector2Temple<T>& Vector2Temple<T>::operator*=(int k)
{
	x *= k;
	y *= k;

	return *this;
}
template<class T>
Vector2Temple<T>& Vector2Temple<T>::operator/=(int k)
{
	x /= k;
	y /= k;

	return *this;
}
template<class T>
Vector2Temple<T> Vector2Temple<T>::operator+() const
{
	//Vector2 vec;

	//vec.x = x;
	//vec.y = y;

	//return vec
	return *this;
}
template<class T>
Vector2Temple<T> Vector2Temple<T>::operator-() const
{
	//Vector2 vec;

	//vec.x = -x;
	//vec.y = -y;
	//
	//return vec;

	return Vector2Temple( -x,-y );
}
template<class T>
Vector2Temple<T>& Vector2Temple<T>::operator+=(int k)
{
	x += k;
	y += k;
	return *this;
	// TODO: return ステートメントをここに挿入します
}
template<class T>
bool Vector2Temple<T>::operator==(const Vector2Temple<T>& vec) const
{
	return (x == vec.x && y == vec.y);
}
template<class T>
bool Vector2Temple<T>::operator!=(const Vector2Temple<T>& vec) const
{
	return (x != vec.x && y != vec.y);
}
template<class T>
bool Vector2Temple<T>::operator<=(const Vector2Temple<T>& vec) const
{
	return (x <= vec.x && y <= vec.y);
}
template<class T>
bool Vector2Temple<T>::operator<(const Vector2Temple<T>& vec) const
{
	return (x < vec.x && y < vec.y);
}
template<class T>
bool Vector2Temple<T>::operator>=(const Vector2Temple<T>& vec) const
{
	return (x >= vec.x && y >= vec.y);
}
template<class T>
bool Vector2Temple<T>::operator>(const Vector2Temple<T>& vec) const
{
	return (x > vec.x && y > vec.y);
}
template<class T>
Vector2Temple<T> operator+(const Vector2Temple<T>& vec, T num)
{
	return Vector2Temple<T>(vec.x + num,vec.y + num);
}
template<class T>
Vector2Temple<T> operator-(const Vector2Temple<T>& vec, T num)
{
	return Vector2Temple<T>(vec.x - num,vec.y - num);
}
template<class T>
Vector2Temple<T> operator*(const Vector2Temple<T>& vec, T num)
{
	return Vector2Temple<T>(vec.x * num, vec.y * num);
}
template<class T>
Vector2Temple<T> operator*(T num, const Vector2Temple<T>& vec)
{
	return Vector2Temple<T>(num * vec.x, num * vec.y);
}
template<class T>
Vector2Temple<T> operator/(const Vector2Temple<T>& vec, T num)
{
	return Vector2Temple<T>(vec.x / num, vec.y / num);
}
template<class T>
Vector2Temple<T> operator%(const Vector2Temple<T>& vec, T num)
{
	return Vector2Temple<T>(vec.x % num, vec.y % num);
}
template<class T>
Vector2Temple<T> operator+(const Vector2Temple<T>& vec1, const Vector2Temple<T>& vec2)
{
	return Vector2Temple<T>(vec1.x+vec2.x,vec1.y + vec2.y);
}
template<class T>
Vector2Temple<T> operator-(const Vector2Temple<T>& vec1, const Vector2Temple<T>& vec2)
{
	return Vector2Temple<T>(vec1.x - vec2.x, vec1.y - vec2.y);
}
