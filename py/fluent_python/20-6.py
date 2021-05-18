import abc


class AutoStorage:
    __count = 0

    def __init__(self):
        cls = self.__class__
        prefix = cls.__name__
        index = cls.__count
        self.storage_name = '_{}#{}'.format(prefix, index)
        cls.__count += 1

    def __get__(self, instance, owner):
        if instance is None:
            return self
        else:
            return getattr(instance, self.storage_name)

    def __set__(self, instance, val):
        setattr(instance, self.storage_name, val)


class Validated(abc.ABC, AutoStorage):
    
    def __set__(self, instance, val):
        val = self.validate(instance, val)
        super().__set__(instance, val)

    @abc.abstractmethod
    def validate(self, instance, val):
        '''return validated value or raise ValueError'''


class Quantity(Validated):

    def validate(self, instance, val):
        if val < 0:
            raise ValueError('value must be > 0')
        return val


class NonBlank(Validated):

    def validate(self, instance, val: str):
        val = val.strip()
        if len(val) == 0:
            raise ValueError('value cannot be empty or blank')
        return val


class LineItem:
    description = NonBlank()
    weight = Quantity()
    height = Quantity()

    def __init__(self, description, weight, height):
        self.description = description
        self.weight = weight
        self.height = height

    def subtotal(self):
        return self.weight * self.height


l = LineItem('ddd', 9, 10)
print(l.subtotal())
l = LineItem('ddd', -9, 10)
l = LineItem('', 9, 10)
